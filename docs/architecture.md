# Architecture

## Request flow

```text
Trace file
   |
   v
TraceParser
   |
   v
vector<MemoryAccess>
   |
   v
SetAssociativeCache::access()
   |
   +-- address mapping
   +-- tag lookup
   +-- hit/miss decision
   +-- replacement policy
   +-- write policy
   +-- miss classification
   +-- statistics update