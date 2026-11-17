# file: Node.py
allNodes = {}

def NodeFactory(node):
    # Handle primitive types (strings, numbers, booleans, None)
    if not isinstance(node, dict):
        return node
    
    # Original code for dict handling
    nodeId = node.get('Node_ID', None)
    
    if nodeId is not None:
        Node(node)  # must create node even if it exists to process new attributes
        return allNodes[nodeId]
    else:
        return Node(node)

class Node(object):

    def __init__(self, node):
        
        #if node already exists, reference it instead of self
        updateNode = self
        nodeId = node.get('Node_ID', None)
        if nodeId is not None:
            if nodeId not in allNodes:
                allNodes[nodeId] = self
            updateNode = allNodes[nodeId]
        
        #update node value for each attribute
        for k in node.keys():
            if isinstance(node[k], dict):
                setattr(updateNode, k, NodeFactory(node[k]))
            elif isinstance(node[k], list):
                l = []
                for item in node[k]:
                    l.append(NodeFactory(item))
                setattr(updateNode, k, l)
            else:
                setattr(updateNode, k, node[k])
    def __str__(self, indent=0, visited=None):
            """Recursively print the node and all its attributes"""
            if visited is None:
                visited = set()
            
            # Check if we've already visited this node (circular reference)
            node_id = getattr(self, 'Node_ID', id(self))
            if node_id in visited:
                return " " * indent + "<Node Node_ID=%s (circular reference)>" % node_id

            visited.add(node_id)
            
            lines = []
            indent_str = " " * indent
            
            # Get all attributes
            attrs = sorted([attr for attr in dir(self) if not attr.startswith('_')])

            
            # Print each attribute
            for attr in attrs:
                value = getattr(self, attr, None)
                
                if isinstance(value, Node):
                    lines.append("%s  %s:" % (indent_str, attr))
                    lines.append(value.__str__(indent + 4, visited))
                elif isinstance(value, list):
                    lines.append("%s  %s: [" % (indent_str, attr))
                    for item in value:
                        if isinstance(item, Node):
                            lines.append(item.__str__(indent + 6, visited))
                        else:
                            lines.append("%s      %r" % (indent_str, item))
                    lines.append("%s  ]" % indent_str)
                else:
                    lines.append("%s  %s: %r" % (indent_str, attr, value))
            
            return "\n".join(lines)