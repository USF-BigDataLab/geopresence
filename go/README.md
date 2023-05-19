## Requirements
1. Check go.mod for required go libraries
1. Check peer C geode for installed shared library requirements
1. Protobuf (verified version 22.3) on PATH

## Building
1. Ensure C geode source files located at ../c/src
1. Run make
  1. Builds C shared library files
  1. Builds protobuf
  1. Builds executable in local folder

## Operation

### Server
1. Copy and modify example_cfg.json for each server instance on each machine
1. "GeohashUpperBound" and "GeohashLowerBound" set the range of geohash this instance will accept for adding
  1. "GeohashUpperBound" is also the node id
  1. Note, lower bound is exclusive, upper bound inclusive
1. The first instance creates the chord ring and shall have "CreateNewChord":true
1. Each other instance will join an existing member
  1. "CreateNewChord":false
  1. "SisterId" set to string id, i.e. GeohashUpperBound of existing node
  1. "SiterHostname" set to string hostname of same existing member, e.g. "host1.com:12300"
1. Note each instance hostname port will also bind to the next higher port
  - Example "host1.com:12300" will bind and listen on ports 12300 and 12301
1. Run "geopresence server path_to_cfg_file.json"
1. Shutdown with SIGINT (Ctrl-C)
1. The majority of updates will be printed to STDOUT with any crash-related reports to STDERR

### Client
1. Add geohashes
  - "geohash client add server_hostname geohash1 geohash2..."
1. Polygon query
  - "geohash client polyq server_hostname lat1 long1 lat2 long2..."
