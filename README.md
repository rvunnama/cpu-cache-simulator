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
   |
   +-- TraceParser
   |      |
   |      +-- MemoryAccess
   |
   +-- SetAssociativeCache
   |      |
   |      +-- CacheSet
   |      |      |
   |      |      +-- CacheLine
   |      |
   |      +-- CacheStatistics
   |      |
   |      +-- MissClassifier
   |
   +-- CacheVisualizer
   |
   +-- BenchmarkRunner

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