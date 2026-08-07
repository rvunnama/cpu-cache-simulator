# CPU Cache Simulator

A configurable CPU cache simulation and analysis tool written in modern C++.

The simulator models direct-mapped and set-associative caches, multiple replacement and write policies, memory traffic, miss classifications, benchmarking, visualization, and performance estimates.

## Features

- Direct-mapped and N-way set-associative caches
- FIFO and LRU replacement policies
- Write-through and write-back behavior
- Write-allocate and no-write-allocate behavior
- Read/write memory traces
- Dirty-bit tracking and dirty evictions
- Compulsory, conflict, capacity, and bypass miss classification
- Memory-read and memory-write statistics
- Traditional AMAT estimation
- Traffic-adjusted average access cost
- Multi-configuration benchmark mode
- CSV export
- Cache-state visualization
- Step-by-step simulation
- Automated tests
- CMake build system

## Technology

- C++17
- CMake
- Standard Template Library
- Git and GitHub

## Architecture

```text
main.cpp
│
├── TraceParser
├── WorkloadGenerator
├── BenchmarkRunner
├── HierarchyBenchmarkRunner
├── ArchitectureComparisonRunner
│
├── SetAssociativeCache
│   ├── CacheSet
│   │   └── CacheLine
│   ├── CacheStatistics
│   ├── MissClassifier
│   └── Replacement Policies
│
├── CacheHierarchy
│   └── HierarchyStatistics
│
└── CacheVisualizer
```
## Included Workloads

The repository contains targeted traces for testing specific cache behaviors:

| Trace | Purpose |
|---|---|
| `basic.trace` | Basic hit/miss behavior |
| `conflict.trace` | Conflict misses |
| `capacity.trace` | Capacity pressure |
| `replacement.trace` | FIFO vs. LRU |
| `locality.trace` | Temporal locality |
| `stride.trace` | Strided access behavior |
| `writeback.trace` | Dirty evictions |
| `mixed.trace` | Mixed read/write behavior |
| `stream.trace` | Streaming workload |

Synthetic workloads can also be generated with `--generate sequential`, `loop`, `stride`, or `random`.

## Performance Results

### Cache Size Scaling

![Hit Rate vs Cache Size](results/hit_rate_vs_cache_size.png)

![Average Access Cost vs Cache Size](results/access_cost_vs_cache_size.png)

### Next-Line Prefetching

![Prefetch Hit Rate](results/prefetch_hit_rate.png)

![Prefetch Memory Traffic](results/prefetch_memory_reads.png)

Next-line prefetching improves sequential demand hit rate by proactively loading the following cache block. The benefit is workload-dependent and is much smaller for random-access patterns.