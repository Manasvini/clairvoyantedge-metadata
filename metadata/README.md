# clairvoyantedge-metadata
Metadata handling for ClairvoyantEdge  

## Metadata required by system  
- [Route](#route)  
- [Segments](#segments)  
- [Nodes](#nodes)  

### Route  
- Keep track of the set of nodes involved in content delivery for a route. Also need to know which video needs to be served along the route   
- route id -> [node1, node2, node3, ...]  
- route id -> video id  
- Delete route once the last segment has been served.    

### Segments  
- Mapping of nodes and segments  
- segment id -> {node1, node4, ...}  
- segment id -> {'size':..., bitrate:.., other properties}  
- Segments can move around depending on cache policies and demand.  

### Nodes  
- Nodes and their location. Needs to be quickly searchable.  
- node id -> lat, long  

