#!/usr/bin/env python3
"""Unit tests for assertion parsing in C_translation.py"""

import unittest
import sys
import os
import re

# Add src directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'src'))

# Try to import, handle Python 3.13+ stricter syntax checking
try:
    import C_translation
except SyntaxError:
    # Python 3.13+ is strict about global declarations - create a mock module
    import types
    C_translation = types.ModuleType('C_translation')
    
    # Read and exec the assertion function only
    src_path = os.path.join(os.path.dirname(__file__), '..', 'src', 'C_translation.py')
    with open(src_path, 'r') as f:
        src = f.read()
    
    # Initialize necessary globals
    C_translation.globalDeclarations = ""
    C_translation.declaredGlobals = set()
    C_translation.finalAssertions = ""
    C_translation.emitHeadersAssertions = []
    
    # Define addGlobalDeclaration
    def addGlobalDeclaration(varName, declaration):
        if varName not in C_translation.declaredGlobals:
            C_translation.declaredGlobals.add(varName)
            C_translation.globalDeclarations += declaration
    C_translation.addGlobalDeclaration = addGlobalDeclaration
    
    # Extract and compile just the assertion function
    match = re.search(r'(def assertion\(assertionString, nodeID\):.*?)(?=\ndef \w+\()', src, re.DOTALL)
    if match:
        assertion_src = match.group(1)
        C_translation.re = re
        exec(compile(assertion_src, '<string>', 'exec'), C_translation.__dict__)


class TestAssertionParsing(unittest.TestCase):
    """Tests for the assertion() function."""
    
    def setUp(self):
        """Reset global state before each test."""
        C_translation.globalDeclarations = ""
        C_translation.declaredGlobals = set()
        C_translation.emitHeadersAssertions = []
    
    # ==========================================================================
    # Basic keywords
    # ==========================================================================
    
    def test_forward_keyword(self):
        """forward -> assert_forward"""
        code, expr = C_translation.assertion("forward", 1000)
        self.assertEqual(code, "")
        self.assertEqual(expr, "assert_forward")
    
    def test_traverse_keyword(self):
        """traverse -> traverse_<nodeID> = 1;"""
        code, expr = C_translation.assertion("traverse", 1000)
        self.assertEqual(code, "traverse_1000 = 1;")
        self.assertEqual(expr, "traverse_1000")
    
    def test_negation_forward(self):
        """!forward -> !(assert_forward)"""
        code, expr = C_translation.assertion("!forward", 1000)
        self.assertEqual(code, "")
        self.assertEqual(expr, "!(assert_forward)")
    
    def test_extract_keyword(self):
        """extract(hdr.ethernet) -> extract_header_hdr_ethernet"""
        code, expr = C_translation.assertion("extract(hdr.ethernet)", 1000)
        self.assertEqual(code, "")
        self.assertEqual(expr, "extract_header_hdr_ethernet")
    
    def test_emit_keyword(self):
        """emit(hdr.ethernet) -> emit_header_hdr_ethernet"""
        code, expr = C_translation.assertion("emit(hdr.ethernet)", 1000)
        self.assertEqual(code, "")
        self.assertEqual(expr, "emit_header_hdr_ethernet")
    
    # ==========================================================================
    # If expressions
    # ==========================================================================
    
    def test_if_traverse_forward(self):
        """if(traverse, forward) -> !(traverse) || (forward)"""
        code, expr = C_translation.assertion("if(traverse, forward)", 1000)
        self.assertEqual(code, "traverse_1000 = 1;")
        self.assertEqual(expr, "!(traverse_1000) || (assert_forward)")
    
    # ==========================================================================
    # AND expressions  
    # ==========================================================================
    
    def test_and_two_forwards(self):
        """forward && forward -> (forward) && (forward)"""
        code, expr = C_translation.assertion("forward && forward", 1000)
        self.assertEqual(code, "")
        self.assertEqual(expr, "(assert_forward) && (assert_forward)")
    
    # ==========================================================================
    # Equality expressions - these have tricky variable name generation
    # ==========================================================================
    
    def test_equality_simple_number(self):
        """hdr.paxos.msgtype == 2"""
        code, expr = C_translation.assertion("hdr.paxos.msgtype == 2", 1000)
        # Variable name format: left.replace(".",_")[:-1] + "_eq_" + right.replace(".","_")[-1:] + "_" + nodeID
        expected_var = "hdr_paxos_msgtype_eq_2_1000"
        self.assertEqual(expr, expected_var)
        self.assertEqual(code, f"{expected_var} = (hdr.paxos.msgtype  ==  2);\n\t")
    
    # ==========================================================================
    # OR expressions
    # ==========================================================================
    
    def test_or_expression(self):
        """a == 3 || a == 4 - should parse like && but with || operator"""
        code, expr = C_translation.assertion("hdr.rtp.timestamp == 3 || hdr.rtp.timestamp == 4", 1000)
        expected_expr = "(hdr_rtp_timestamp_eq_3_1000) || (hdr_rtp_timestamp_eq_4_1000)"
        self.assertEqual(expr, expected_expr)
    
    # ==========================================================================
    # Parenthesized expressions - needs fix
    # ==========================================================================
    
    def test_parenthesized_or_expression(self):
        """(a == 3 || a == 4) - should strip parens and parse inner expression"""
        code, expr = C_translation.assertion("(hdr.rtp.timestamp == 3 || hdr.rtp.timestamp == 4)", 1000)
        expected_expr = "(hdr_rtp_timestamp_eq_3_1000) || (hdr_rtp_timestamp_eq_4_1000)"
        self.assertEqual(expr, expected_expr)
    
    def test_negated_parenthesized_or(self):
        """!(a == 3 || a == 4) - the actual ts_switching case"""
        code, expr = C_translation.assertion("!(hdr.rtp.timestamp == 3 || hdr.rtp.timestamp == 4)", 1000)
        expected_expr = "!((hdr_rtp_timestamp_eq_3_1000) || (hdr_rtp_timestamp_eq_4_1000))"
        self.assertEqual(expr, expected_expr)


if __name__ == '__main__':
    unittest.main(verbosity=2)
