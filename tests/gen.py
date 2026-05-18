#!/usr/bin/env python3
"""
Random test case generator for Hackathon Squad (MWIS).

Usage:
    python3 gen.py <N> <edge_density> [seed]

Arguments:
    N             Number of coders (e.g. 10, 1000, 200000)
    edge_density  Fraction of all possible edges to include, 0.0 – 1.0
                  (0.01 = sparse/realistic, 0.5 = dense, 1.0 = complete graph)
    seed          Optional random seed for reproducibility (default: 42)

Examples:
    # Small readable test
    python3 gen.py 10 0.3 7

    # Stress test near the maximum constraints
    python3 gen.py 200000 0.000005 42

    # Dense graph (most nodes will be forced out)
    python3 gen.py 500 0.5 1
"""

import random
import sys


def main():
    if len(sys.argv) < 3:
        print(__doc__, file=sys.stderr)
        sys.exit(1)

    N            = int(sys.argv[1])
    edge_density = float(sys.argv[2])
    seed         = int(sys.argv[3]) if len(sys.argv) > 3 else 42

    random.seed(seed)

    # Skill ratings: uniform in [1, 10^9]
    skills = [random.randint(1, 1_000_000_000) for _ in range(N)]

    # Generate edges
    max_edges = N * (N - 1) // 2
    target_M  = min(max_edges, int(max_edges * edge_density))

    if target_M == max_edges:
        # Complete graph — list all edges
        edges = [(u, v) for u in range(1, N + 1) for v in range(u + 1, N + 1)]
    else:
        # Sample without replacement from all possible edges
        # For large N, rejection-sampling is more memory-efficient than
        # materialising all pairs.
        edges = set()
        attempts = 0
        max_attempts = target_M * 20  # give up after this many tries

        while len(edges) < target_M and attempts < max_attempts:
            u = random.randint(1, N)
            v = random.randint(1, N)
            if u != v:
                edge = (min(u, v), max(u, v))
                edges.add(edge)
            attempts += 1

        edges = list(edges)

    M = len(edges)

    # Output
    print(N, M)
    print(*skills)
    for u, v in edges:
        print(u, v)


if __name__ == "__main__":
    main()
