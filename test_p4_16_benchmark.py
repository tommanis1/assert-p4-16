#!/usr/bin/env python3
"""
Comprehensive test suite for Assert-P4 P4-16 support.

Tests are structured in levels:
1. Unit tests: Test individual C_translation.py functions
2. Integration tests: Test P4 -> JSON -> C pipeline compiles
3. Regression tests: Compare generated C against known-good reference models
"""

import subprocess
import sys
import os
import tempfile
import shutil
import difflib
import argparse
import re
import json
from pathlib import Path
from dataclasses import dataclass, field
from typing import Optional, List, Tuple
from enum import Enum, auto


# Path configuration
SCRIPT_DIR = Path(__file__).parent.absolute()
SRC_DIR = SCRIPT_DIR / "src"
EXPERIMENTS_DIR = SCRIPT_DIR / "experiments" / "case_studies"
DOCKER_IMAGE = "assert-p4:latest"


class TestLevel(Enum):
    UNIT = auto()
    INTEGRATION = auto()
    REGRESSION = auto()


@dataclass
class TestCase:
    """A test case with P4 source and expected reference C model."""
    name: str
    p4_file: Path
    reference_c: Optional[Path] = None
    description: str = ""


# P4-16 test cases with their reference C models from original experiments
P4_16_TEST_CASES = [
    TestCase(
        name="paxos_acceptor",
        p4_file=EXPERIMENTS_DIR / "paxos/p4-16/p4src/acceptor.p4",
        reference_c=EXPERIMENTS_DIR / "paxos/p4-16/p4src/acceptor.c",
        description="Paxos consensus acceptor"
    ),
    TestCase(
        name="paxos_leader",
        p4_file=EXPERIMENTS_DIR / "paxos/p4-16/p4src/leader.p4",
        reference_c=EXPERIMENTS_DIR / "paxos/p4-16/p4src/leader.c",
        description="Paxos consensus leader"
    ),
    TestCase(
        name="paxos_learner",
        p4_file=EXPERIMENTS_DIR / "paxos/p4-16/p4src/learner.p4",
        reference_c=EXPERIMENTS_DIR / "paxos/p4-16/p4src/learner.c",
        description="Paxos consensus learner"
    ),
    TestCase(
        name="stag",
        p4_file=EXPERIMENTS_DIR / "stag/stag.p4",
        reference_c=EXPERIMENTS_DIR / "stag/stag.c",
        description="Source Tag routing"
    ),
    TestCase(
        name="ts_switching",
        p4_file=EXPERIMENTS_DIR / "ts_switching/ts_switching-16.p4",
        reference_c=EXPERIMENTS_DIR / "ts_switching/ts_switching-16.c",
        description="Timestamp-based switching"
    )
]


# =============================================================================
# Docker helpers
# =============================================================================

def run_p4c_docker(p4_file: Path, output_json: Path) -> Tuple[bool, str]:
    """Run p4c in Docker to compile P4 to JSON."""
    p4_dir = p4_file.parent
    p4_name = p4_file.name
    json_name = output_json.name
    output_dir = output_json.parent
    
    cmd = [
        "docker", "run", "--rm",
        "-v", f"{p4_dir}:/input:ro",
        "-v", f"{output_dir}:/output",
        "--entrypoint", "/assert-p4/bin/p4c-bm2-ss",
        DOCKER_IMAGE,
        "--toJSON", f"/output/{json_name}",
        f"/input/{p4_name}"
    ]
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
        if result.returncode == 0:
            return True, ""
        return False, result.stderr
    except Exception as e:
        return False, str(e)


def run_p4_to_c_docker(json_file: Path) -> Tuple[bool, str, str]:
    """Run P4_to_C.py in Docker to generate C code from JSON."""
    json_dir = json_file.parent
    json_name = json_file.name
    
    cmd = [
        "docker", "run", "--rm",
        "-v", f"{json_dir}:/input:ro",
        "-v", f"{SRC_DIR}:/assert-p4/src:ro",  # Mount local src/ for testing
        "--entrypoint", "python",
        DOCKER_IMAGE,
        "/assert-p4/src/P4_to_C.py",
        f"/input/{json_name}"
    ]
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=120)
        if result.returncode == 0:
            return True, result.stdout, ""
        return False, "", result.stderr
    except Exception as e:
        return False, "", str(e)


def run_entrypoint_docker(p4_file: Path, output_dir: Path, timeout: int = 300) -> Tuple[bool, dict]:
    """Run the full assert-p4 entrypoint in Docker (P4->JSON->C->LLVM->KLEE).
    
    Returns (success, result_dict) where result_dict contains the JSON output from entrypoint.
    Saves the result to output_dir/entrypoint_result.json
    """
    p4_dir = p4_file.parent
    p4_name = p4_file.name
    
    cmd = [
        "docker", "run", "--rm",
        "-v", f"{p4_dir}:/input:ro",
        "-v", f"{SRC_DIR}:/assert-p4/src:ro",  # Mount local src/ for testing
        DOCKER_IMAGE,
        f"/input/{p4_name}",
        str(timeout)
    ]
    
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout + 60)
        
        # Parse JSON from stdout
        try:
            result_dict = json.loads(result.stdout.strip())
        except json.JSONDecodeError:
            result_dict = {
                "verdict": "error",
                "details": f"Failed to parse JSON output: {result.stdout[:500]}",
                "stderr": result.stderr[:500]
            }
        
        # Save result to output directory
        result_file = output_dir / "entrypoint_result.json"
        with open(result_file, 'w') as f:
            json.dump(result_dict, f, indent=2)
        
        success = result_dict.get("verdict") in ["true", "false"]
        return success, result_dict
        
    except subprocess.TimeoutExpired:
        result_dict = {"verdict": "timeout", "details": f"Entrypoint timed out after {timeout}s"}
        result_file = output_dir / "entrypoint_result.json"
        with open(result_file, 'w') as f:
            json.dump(result_dict, f, indent=2)
        return False, result_dict
    except Exception as e:
        result_dict = {"verdict": "error", "details": str(e)}
        return False, result_dict


def compile_c_code(c_code: str) -> Tuple[bool, str]:
    """Check if C code compiles with clang."""
    stubs = """
void klee_make_symbolic(void *addr, unsigned long nbytes, const char *name) {}
void klee_abort(void) {}
void klee_assume(int condition) {}
void klee_assert(int condition) {}
void klee_warning(const char *msg) {}
void klee_warning_once(const char *msg) {}
void klee_print_once(int id, const char *msg) {}
"""
    full_code = stubs + c_code
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.c', delete=False) as f:
        f.write(full_code)
        temp_file = f.name
    
    try:
        result = subprocess.run(
            ["clang", "-fsyntax-only", "-Wno-parentheses-equality", temp_file],
            capture_output=True, text=True
        )
        os.unlink(temp_file)
        
        if result.returncode == 0:
            return True, ""
        errors = [l for l in result.stderr.split('\n') if 'error:' in l]
        return False, errors[0] if errors else result.stderr.split('\n')[0]
    except FileNotFoundError:
        os.unlink(temp_file)
        return None, "clang not found"


# =============================================================================
# Diffing and comparison utilities
# =============================================================================

def normalize_c_code(code: str) -> List[str]:
    """Normalize C code for comparison (remove whitespace variations, comments)."""
    lines = []
    for line in code.split('\n'):
        # Remove single-line comments
        line = re.sub(r'//.*$', '', line)
        # Strip whitespace
        line = line.strip()
        # Skip empty lines
        if line:
            lines.append(line)
    return lines


def generate_diff(generated: str, reference: str, context: int = 3) -> List[str]:
    """Generate a unified diff between generated and reference C code."""
    gen_lines = normalize_c_code(generated)
    ref_lines = normalize_c_code(reference)
    
    diff = list(difflib.unified_diff(
        ref_lines, gen_lines,
        fromfile='reference.c',
        tofile='generated.c',
        lineterm='',
        n=context
    ))
    return diff


def extract_functions(code: str) -> set:
    """Extract function names from C code."""
    return set(re.findall(r'void\s+(\w+)\s*\(', code))


def extract_structs(code: str) -> set:
    """Extract struct names from C code."""
    return set(re.findall(r'typedef\s+struct\s*\{[^}]+\}\s*(\w+)', code))


def compare_structure(generated: str, reference: str) -> dict:
    """Compare structural elements between generated and reference C."""
    gen_funcs = extract_functions(generated)
    ref_funcs = extract_functions(reference)
    gen_structs = extract_structs(generated)
    ref_structs = extract_structs(reference)
    
    return {
        'missing_functions': sorted(ref_funcs - gen_funcs),
        'extra_functions': sorted(gen_funcs - ref_funcs),
        'missing_structs': sorted(ref_structs - gen_structs),
        'extra_structs': sorted(gen_structs - ref_structs),
        'common_functions': len(ref_funcs & gen_funcs),
        'common_structs': len(ref_structs & gen_structs),
    }


# =============================================================================
# Test Results
# =============================================================================

@dataclass 
class TestResult:
    """Result of a test case."""
    name: str
    level: TestLevel
    passed: bool = False
    error: str = ""
    # Integration level
    p4c_success: bool = False
    p4_to_c_success: bool = False
    c_compiles: bool = False
    generated_c: str = ""
    # Regression level
    diff_lines: List[str] = field(default_factory=list)
    structure_diff: dict = field(default_factory=dict)
    # Entrypoint (full KLEE verification)
    entrypoint_result: dict = field(default_factory=dict)
    
    def short_status(self) -> str:
        if self.passed:
            return "✓"
        return "✗"


# =============================================================================
# Test runners for each level
# =============================================================================

def run_integration_test(tc: TestCase) -> TestResult:
    """Run integration test: P4 -> JSON -> C pipeline."""
    result = TestResult(name=tc.name, level=TestLevel.INTEGRATION)
    
    if not tc.p4_file.exists():
        result.error = f"P4 file not found: {tc.p4_file}"
        return result
    
    with tempfile.TemporaryDirectory() as tmpdir:
        json_file = Path(tmpdir) / "output.json"
        
        # Step 1: P4 -> JSON
        result.p4c_success, p4c_error = run_p4c_docker(tc.p4_file, json_file)
        if not result.p4c_success:
            result.error = f"P4C failed: {p4c_error[:80]}"
            return result
        
        # Step 2: JSON -> C
        result.p4_to_c_success, c_code, p4_to_c_error = run_p4_to_c_docker(json_file)
        if not result.p4_to_c_success:
            result.error = f"P4_to_C failed: {p4_to_c_error[:80]}"
            return result
        
        result.generated_c = c_code
        
        # Step 3: Check C compiles
        compiles, compile_error = compile_c_code(c_code)
        if compiles is None:
            result.c_compiles = True  # Skip if clang unavailable
        else:
            result.c_compiles = compiles
            if not compiles:
                result.error = f"C compile failed: {compile_error[:80]}"
                return result
    
    result.passed = True
    
    # Step 4: Run full entrypoint (KLEE verification) and save result
    output_dir = SCRIPT_DIR / "benchmark_output" / tc.name
    output_dir.mkdir(parents=True, exist_ok=True)
    
    # Save generated C to benchmark output
    generated_c_file = output_dir / "generated.c"
    generated_c_file.write_text(result.generated_c)
    
    print("[KLEE]", end=" ", flush=True)
    entrypoint_success, entrypoint_result_dict = run_entrypoint_docker(
        tc.p4_file, output_dir, timeout=120
    )
    result.entrypoint_result = entrypoint_result_dict
    
    return result


def run_regression_test(tc: TestCase, output_dir: Path = None) -> TestResult:
    """Run regression test: generate C and diff against reference using Unix diff.
    
    Saves artifacts to output_dir/{test_name}/:
    - output.json: P4C JSON output
    - generated.c: Generated C code
    - reference.c: Copy of reference C code
    - compile.log: Compilation log (if any)
    - diff.txt: Unified diff output
    """
    result = TestResult(name=tc.name, level=TestLevel.REGRESSION)
    
    if not tc.p4_file.exists():
        result.error = f"P4 file not found: {tc.p4_file}"
        return result
    
    if not tc.reference_c or not tc.reference_c.exists():
        result.error = "No reference C file available"
        return result
    
    # Create output directory for this test
    if output_dir is None:
        output_dir = SCRIPT_DIR / "benchmark_output"
    test_output_dir = output_dir / tc.name
    if test_output_dir.exists():
        shutil.rmtree(test_output_dir)
    test_output_dir.mkdir(parents=True, exist_ok=True)
    
    json_file = test_output_dir / "output.json"
    generated_c_file = test_output_dir / "generated.c"
    reference_c_file = test_output_dir / "reference.c"
    compile_log_file = test_output_dir / "compile.log"
    diff_file = test_output_dir / "diff.txt"
    
    compile_log = []
    
    # Step 1: P4 -> JSON
    result.p4c_success, p4c_error = run_p4c_docker(tc.p4_file, json_file)
    compile_log.append(f"=== P4C Compilation ===\n")
    if result.p4c_success:
        compile_log.append("SUCCESS\n")
    else:
        compile_log.append(f"FAILED:\n{p4c_error}\n")
        result.error = f"P4C failed: {p4c_error[:80]}"
        compile_log_file.write_text(''.join(compile_log))
        return result
    
    # Step 2: JSON -> C
    result.p4_to_c_success, c_code, p4_to_c_error = run_p4_to_c_docker(json_file)
    compile_log.append(f"\n=== P4_to_C Translation ===\n")
    if result.p4_to_c_success:
        compile_log.append("SUCCESS\n")
    else:
        compile_log.append(f"FAILED:\n{p4_to_c_error}\n")
        result.error = f"P4_to_C failed: {p4_to_c_error[:80]}"
        compile_log_file.write_text(''.join(compile_log))
        return result
    
    result.generated_c = c_code
    
    # Save generated C
    generated_c_file.write_text(c_code)
    
    # Copy reference C
    reference_c_file.write_text(tc.reference_c.read_text())
    
    # Step 3: Run diff command
    try:
        diff_result = subprocess.run(
            ["diff", "-u", str(reference_c_file), str(generated_c_file)],
            capture_output=True, text=True
        )
        result.diff_lines = diff_result.stdout.split('\n') if diff_result.stdout else []
        
        # Save diff output
        diff_file.write_text(diff_result.stdout if diff_result.stdout else "Files are identical\n")
        
        compile_log.append(f"\n=== Diff Result ===\n")
        
        # diff returns 0 if identical, 1 if different, 2 if error
        if diff_result.returncode == 0:
            result.passed = True
            compile_log.append("IDENTICAL\n")
        elif diff_result.returncode == 1:
            result.passed = False
            num_diffs = len([l for l in result.diff_lines if l.startswith('+') or l.startswith('-')])
            result.error = f"{num_diffs} lines differ from reference"
            compile_log.append(f"DIFFERS: {num_diffs} lines\n")
        else:
            result.error = f"diff error: {diff_result.stderr}"
            compile_log.append(f"ERROR: {diff_result.stderr}\n")
    except Exception as e:
        result.error = f"diff failed: {e}"
        compile_log.append(f"EXCEPTION: {e}\n")
    
    # Save compile log
    compile_log_file.write_text(''.join(compile_log))
    
    return result


# =============================================================================
# Main test runner
# =============================================================================

def run_tests(level: TestLevel, verbose: bool = False, show_diff: bool = False) -> List[TestResult]:
    """Run all tests at specified level."""
    results = []
    
    for tc in P4_16_TEST_CASES:
        print(f"Testing: {tc.name}...", end=" ", flush=True)
        
        if level == TestLevel.INTEGRATION:
            result = run_integration_test(tc)
        else:  # REGRESSION
            result = run_regression_test(tc)
        
        results.append(result)
        print(result.short_status())
        
        if verbose and not result.passed:
            print(f"    Error: {result.error}")
        
        if show_diff and result.diff_lines:
            print("\n--- Diff (reference vs generated) ---")
            for line in result.diff_lines[:50]:  # Limit output
                print(line)
            if len(result.diff_lines) > 50:
                print(f"... ({len(result.diff_lines) - 50} more lines)")
            print("--- End diff ---\n")
    
    return results


def print_summary(results: List[TestResult], show_structure: bool = False):
    """Print test summary."""
    passed = sum(1 for r in results if r.passed)
    failed = len(results) - passed
    
    print("\n" + "=" * 70)
    print(f"SUMMARY: {passed}/{len(results)} passed, {failed} failed")
    print("=" * 70)
    
    if failed > 0:
        print("\nFAILURES:")
        for r in results:
            if not r.passed:
                print(f"  ✗ {r.name}: {r.error}")
    
    if show_structure:
        print("\nSTRUCTURE COMPARISON:")
        for r in results:
            if r.structure_diff:
                sd = r.structure_diff
                print(f"\n  {r.name}:")
                print(f"    Functions: {sd['common_functions']} common, "
                      f"{len(sd['missing_functions'])} missing, {len(sd['extra_functions'])} extra")
                print(f"    Structs: {sd['common_structs']} common, "
                      f"{len(sd['missing_structs'])} missing, {len(sd['extra_structs'])} extra")
                if sd['missing_functions']:
                    print(f"    Missing funcs: {', '.join(sd['missing_functions'][:5])}")


def main():
    parser = argparse.ArgumentParser(description="Assert-P4 P4-16 Test Suite")
    parser.add_argument('--level', choices=['unit', 'integration', 'regression', 'all'], 
                        default='all', help='Test level to run (default: all)')
    parser.add_argument('-v', '--verbose', action='store_true', help='Verbose output')
    parser.add_argument('--diff', action='store_true', help='Show diffs for regression tests')
    parser.add_argument('--structure', action='store_true', help='Show structure comparison')
    args = parser.parse_args()
    
    print("=" * 70)
    print("Assert-P4 P4-16 Test Suite")
    print("=" * 70)
    
    all_results = []
    total_failed = 0
    
    levels_to_run = []
    if args.level == 'all':
        levels_to_run = [TestLevel.UNIT, TestLevel.INTEGRATION, TestLevel.REGRESSION]
    elif args.level == 'unit':
        levels_to_run = [TestLevel.UNIT]
    elif args.level == 'integration':
        levels_to_run = [TestLevel.INTEGRATION]
    else:
        levels_to_run = [TestLevel.REGRESSION]
    
    # Level 1: Unit tests
    if TestLevel.UNIT in levels_to_run:
        print("\n" + "-" * 70)
        print("Level 1: UNIT TESTS (C_translation.py functions)")
        print("-" * 70)
        print("  (Not yet implemented - TODO: add unit tests for assertion(), toC(), etc.)")
    
    # Level 2: Integration tests
    if TestLevel.INTEGRATION in levels_to_run:
        print("\n" + "-" * 70)
        print("Level 2: INTEGRATION TESTS (P4 -> JSON -> C pipeline)")
        print("-" * 70)
        results = run_tests(TestLevel.INTEGRATION, verbose=args.verbose, show_diff=False)
        all_results.extend(results)
        passed = sum(1 for r in results if r.passed)
        failed = len(results) - passed
        total_failed += failed
        print(f"\n  Integration: {passed}/{len(results)} passed")
        if failed > 0:
            for r in results:
                if not r.passed:
                    print(f"    ✗ {r.name}: {r.error}")
    
    # Level 3: Regression tests
    if TestLevel.REGRESSION in levels_to_run:
        print("\n" + "-" * 70)
        print("Level 3: REGRESSION TESTS (compare vs reference C models)")
        print("-" * 70)
        results = run_tests(TestLevel.REGRESSION, verbose=args.verbose, show_diff=args.diff)
        all_results.extend(results)
        passed = sum(1 for r in results if r.passed)
        failed = len(results) - passed
        total_failed += failed
        print(f"\n  Regression: {passed}/{len(results)} passed")
        if failed > 0:
            for r in results:
                if not r.passed:
                    print(f"    ✗ {r.name}: {r.error}")
        
        if args.structure:
            print_summary(results, show_structure=True)
    
    # Final summary
    print("\n" + "=" * 70)
    if total_failed == 0:
        print("ALL TESTS PASSED")
    else:
        print(f"FAILED: {total_failed} test(s) failed")
    print("=" * 70)
    
    # Print output directory info for regression tests
    if TestLevel.REGRESSION in levels_to_run:
        print("\nTest artifacts saved to:")
        for tc in P4_16_TEST_CASES:
            test_dir = Path("benchmark_output") / tc.name
            if test_dir.exists():
                print(f"  {test_dir}/")
                for f in ["output.json", "generated.c", "reference.c", "diff.txt", "compile.log"]:
                    fpath = test_dir / f
                    if fpath.exists():
                        print(f"    {fpath}")
    
    return 0 if total_failed == 0 else 1


if __name__ == "__main__":
    sys.exit(main())

