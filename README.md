# Hackathon Squad 🏆

> **IIT Guwahati Coding Club — Even Semester Projects 2026**  

---

## Problem Statement

You are assembling a dream team from **N** freshman coders for a hackathon.  
Each coder has a **skill rating**. Certain pairs of coders **refuse to work together** (conflict pairs).

**Goal:** Select a conflict-free subset (**Independent Set**) that maximises the **total skill rating**, within a **5-minute execution window**.

This is the **Maximum Weight Independent Set (MWIS)** problem — provably NP-hard, so we use fast heuristics.

---

## Algorithm

### Why Heuristics?

For N = 200,000 nodes, an exact solver would take exponential time. Instead, we run multiple smart strategies within the time budget and keep the best answer found.

### Three-Phase Strategy

```
Phase 1 ─ Greedy Seeds
          Build four initial solutions using different node orderings:
          ① skill ÷ (degree + 1)  ratio  (best all-round)
          ② raw skill  (descending)
          ③ degree     (ascending — low-conflict nodes first)
          Keep the best solution found.

Phase 2 ─ 1-Swap Local Search
          For each selected node u (cheapest first):
            • Temporarily remove u
            • Greedily add the free neighbours of u (by skill descending)
            • Keep the swap only if   Σ gained > skill[u]
          Repeat until no improvement is possible.

Phase 3 ─ Iterated Greedy  (runs until 280 s elapsed)
          Repeat:
            • Randomly destroy 20–60 % of the current solution
            • Rebuild greedily (ratio order)
            • Apply a quick 1-swap polish
            • If better than global best → update
            • If stuck for 25 rounds → increase destroy rate (more exploration)
```

### Complexity per Phase

| Phase | Time per iteration |
|---|---|
| Greedy seed | O(N log N + M) |
| 1-swap pass | O(N · avg_degree²) |
| Iterated Greedy rebuild | O(N log N + M) |

For **sparse** graphs (typical for hackathon inputs) all phases are fast enough to run hundreds of iterations in the 5-minute window.

---

## Input / Output Format

### Input
```
N M
S₁ S₂ ... Sₙ
u₁ v₁
u₂ v₂
...
uₘ vₘ
```

| Symbol | Meaning |
|---|---|
| N | Number of coders (1 ≤ N ≤ 200,000) |
| M | Number of conflict pairs (0 ≤ M ≤ N·(N−1)/2) |
| Sᵢ | Skill rating of coder i (1 ≤ Sᵢ ≤ 10⁹) |
| uⱼ vⱼ | Mutual conflict between coder uⱼ and coder vⱼ |

### Output
```
<total skill of selected team>
<space-separated indices of selected coders, ascending>
```

---

## Build & Run

### Prerequisites
- `g++` with C++17 support (GCC 7+ or Clang 5+)
- `make` (optional, for the Makefile)
- `python3` (optional, for the test generator)

### Compile with Make
```bash
make
```

### Compile manually
```bash
g++ -O2 -std=c++17 -Wall -o hackathon_squad main.cpp
```

### Run
```bash
./hackathon_squad < tests/test1.txt
# or pipe your own input:
echo "3 1
10 5 8
1 2" | ./hackathon_squad
```

### Run all bundled tests
```bash
make tests
```

### Stress test (N = 200,000)
```bash
make stress
```

---

## Test Cases

| File | N | M | Description | Expected score |
|---|---|---|---|---|
| `test1.txt` | 5 | 4 | Small tree-like graph | 20 |
| `test2.txt` | 7 | 7 | Odd cycle C₇ | 430 |
| `test3.txt` | 1 | 0 | Single node (edge case) | 999999999 |
| `test4.txt` | 4 | 6 | Complete graph K₄ | 400 |

Generate a random test of your own:
```bash
# python3 tests/gen.py <N> <edge_density_0_to_1> <random_seed>
python3 tests/gen.py 1000 0.01 99 > my_test.txt
./hackathon_squad < my_test.txt
```

---

## File Structure

```
hackathon-squad/
├── main.cpp              ← complete solver (single file, submit this)
├── Makefile              ← build + test shortcuts
├── CMakeLists.txt        ← alternative CMake build
├── .gitignore
├── README.md
└── tests/
    ├── gen.py            ← random test generator
    ├── test1.txt         ← small graph
    ├── test1_expected.txt
    ├── test2.txt         ← cycle C₇
    ├── test2_expected.txt
    ├── test3.txt         ← single node
    ├── test3_expected.txt
    ├── test4.txt         ← complete graph K₄
    └── test4_expected.txt
```

---

## Key Concepts to Study

| Topic | Why it matters here |
|---|---|
| **Graph theory** — Independent Set | The core problem definition |
| **Greedy algorithms** | Phase 1 and the rebuild step |
| **Local search / Hill climbing** | Phase 2 (1-swap) |
| **Iterated Greedy metaheuristic** | Phase 3 escape from local optima |
| **Amortised complexity** | Understanding why `fillFree` is O(N+M) not O(N²) |

---

## Notes on Scoring

Your submission is judged on:
1. **Validity** — the selected set must be strictly conflict-free.
2. **Score** — higher total skill is better, relative to the best known solution.

The validator will check both. An invalid team (even with a huge score) is disqualified.
