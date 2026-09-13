# Cache Hierarchy Simulator

A trace-driven simulator for a two-level (L1 + L2) set-associative cache
hierarchy: given a memory access trace, it models both caches cycle by
cycle and reports hits, misses, and writebacks at each level.

## How it works

- **Cache model**: each cache (`struct Cache` in `simulator.cpp`) is a
  set-associative array of `[valid, dirty, tag, last-used-counter]` lines.
  Replacement is **LRU**, tracked via a global access counter stamped on
  every hit/fill; the line with the smallest counter in a set is evicted.
- **Write policy**: write-back, write-allocate. A dirty line evicted from
  L1 is written into L2 (and L2 in turn evicts to memory if needed); an L2
  eviction invalidates the corresponding line in L1 if present.
- **Cost model** (`tools/sweep_experiment.cpp`): 1 cycle per L1 access,
  20 cycles per L2 access or L1→L2 writeback, 200 cycles per L2 miss
  (memory access) or L2→memory writeback.

## Trace file format

One access per line: `<r|w> <hex address>`, e.g.:

```
r 0x1000
w 0x1000
r 0x2000
```

**Known limitation**: the trace reader uses a `while(!eof())` loop, so a
trace file that ends with a trailing blank line (i.e. a final newline
character — the normal way most editors/tools save text files) makes it
try to parse an empty line and crash with `stoll: no conversion`. Make
sure your trace file has no trailing newline after the last access, or
strip one with `printf '%s' "$(cat trace.txt)" > trace_fixed.txt`.

## Build & run

```bash
g++ -std=c++17 -O2 -o simulator simulator.cpp
./simulator <block_size> <l1_size> <l1_assoc> <l2_size> <l2_assoc> <trace_file>

# e.g. 64B blocks, 1KB 2-way L1, 64KB 8-way L2
./simulator 64 1024 2 65536 8 trace.txt
```

All sizes are in bytes; block/L1/L2 sizes must divide evenly into a whole
number of sets given the associativity (`size / (block_size * assoc)`).

Output is hit/miss/writeback counts for L1 then L2:

```
 final answer
 for l1
reads 4
readmiss 3
writes 2
writemiss 1
writeback 1
 for l2
reads 4
readmiss 4
writes 1
writemiss 0
writeback 0
```

## `tools/` — parameter sweep & plots

- `sweep_experiment.cpp` — the same cache model, wrapped so it can be
  called as `program(block_size, l1_size, l1_assoc, l2_size, l2_assoc, trace_path)`
  and returns total simulated access time under the cost model above.
  `main()` sweeps one parameter at a time (block size, L1 size, L1
  associativity, L2 size, L2 associativity) across 5 preset values while
  holding the rest at a default configuration (64B / 1KB / 2-way / 64KB /
  8-way), against up to 8 trace files, appending results as CSV lines to
  `trace_answer.txt`.
- `plot_results.py` — reads `trace_answer.txt` and bar-charts each of the
  5 sweeps (access time vs. block size / L1 size / L1 assoc / L2 size /
  L2 assoc) for the **first** trace file's results.

```bash
g++ -std=c++17 -O2 -o tools/sweep_experiment tools/sweep_experiment.cpp
cd tools
rm -f trace_answer.txt   # it appends, so start fresh
./sweep_experiment trace1.txt trace2.txt ... trace8.txt   # needs 8 trace files
python3 plot_results.py
```

## `archive/`

Earlier iterations of the simulator, kept to show how it evolved (fixing
LRU bugs, trying an inclusive cache policy, etc.) — not used by anything
in `tools/` or the build above.
