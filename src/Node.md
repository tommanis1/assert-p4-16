# Node.py

Node.py converts p4c JSON output into a graph of interconnected Node objects, with automatic deduplication based on Node_ID.

## Key Components

### 1. Global Node Registry
`allNodes = {}`  # Stores all nodes by their Node_ID for reference management

### 2. NodeFactory Function
- Acts as a factory pattern for creating/retrieving nodes
- Ensures nodes with the same Node_ID return the same object instance (singleton per ID)
- Handles error recovery when accessing Node_ID

### 3. Node Class
Converts dictionary data into Node objects with these features:
- **Deduplication**: If a Node_ID already exists, updates the existing node instead of creating a new one
- **Recursive processing**:
  - Nested dicts → become nested Node objects
  - Lists → become lists of Node objects
  - Other values → become direct attributes


There orginal Node.py assumes all items in lists are dictionaries (with potential Node_ID fields), but the new p4c JSON structure has lists containing primitive values (strings, numbers, etc.) rather than just dictionaries.

