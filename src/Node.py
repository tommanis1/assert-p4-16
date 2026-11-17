
allNodes = {}

def NodeFactory(node):
    nodeId = node.get('Node_ID', None)
    if nodeId is not None:
        Node(node) #must create node even if it exists to process new attributes
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

    def __str__(self):
            """String representation for debugging."""
            if hasattr(self, 'Node_Type'):
                return "<Node: %s>" % self.Node_Type
            elif hasattr(self, 'value'):
                return "<Node: %s>" % self.value
            else:
                return "<Node>"
    
    def __repr__(self):
        return self.__str__()
    
    def show(self, indent=0, max_depth=3):
        """Pretty print the node structure for debugging."""
        prefix = "  " * indent
        
        # Avoid infinite recursion
        if indent > max_depth:
            print prefix + "..."
            return
        
        # Print node type
        if hasattr(self, 'Node_Type'):
            print prefix + "Node_Type: %s" % self.Node_Type
            if hasattr(self, 'Node_ID'):
                print prefix + "Node_ID: %s" % self.Node_ID
        else:
            print prefix + "Node (no type)"
        
        # Print attributes
        for attr in dir(self):
            if attr.startswith('_') or attr in ['show', 'Node_Type', 'Node_ID']:
                continue
                
            val = getattr(self, attr)
            
            # Skip methods
            if callable(val):
                continue
            
            # Handle different types of values
            if isinstance(val, Node):
                print prefix + "%s:" % attr
                val.show(indent + 1, max_depth)
            elif isinstance(val, list):
                if len(val) > 0:
                    print prefix + "%s: [%d items]" % (attr, len(val))
                    # Show first few items if they're Nodes
                    for i, item in enumerate(val[:3]):  # Show first 3 items
                        if isinstance(item, Node):
                            print prefix + "  [%d]:" % i
                            item.show(indent + 2, max_depth)
                        else:
                            print prefix + "  [%d]: %s" % (i, str(item)[:50])
                    if len(val) > 3:
                        print prefix + "  ..."
                else:
                    print prefix + "%s: []" % attr
            elif isinstance(val, (str, unicode)):
                # Truncate long strings
                if len(val) > 50:
                    print prefix + "%s: %s..." % (attr, val[:50])
                else:
                    print prefix + "%s: %s" % (attr, val)
            else:
                print prefix + "%s: %s" % (attr, str(val)[:100])