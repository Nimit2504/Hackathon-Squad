/*
 * HACKATHON SQUAD — Maximum Weight Independent Set
 * IIT Guwahati Coding Club - Even Semester Projects 2026
 *
 * Problem: NP-hard (reduces from Max Independent Set), so we use heuristics.
 *
 * Strategy:
 *   1. Run greedy with three node orderings, keep the best result.
 *      - skill / (degree+1) ratio  — balances value vs conflict cost
 *      - raw skill descending      — pure value-first
 *      - degree ascending          — low-conflict nodes first
 *   2. Improve with 1-swap local search + free-node fill pass.
 *   3. Iterated Greedy: randomly destroy part of the solution and rebuild.
 *      Escapes local optima that 1-swap alone cannot cross.
 *      Runs until the 5-minute wall.
 *
 * Key design: conflictCnt[u] = # selected neighbours of u.
 *   canAdd(u) is O(1) — just check conflictCnt[u] == 0.
 *   add / remove update conflictCnt in O(degree).
 */

#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

// ── Timer ─────────────────────────────────────────────────────────────────────
auto startTime = chrono::steady_clock::now();
double elapsed() {
    return chrono::duration<double>(
        chrono::steady_clock::now() - startTime).count();
}

// ── Graph (global, read-only after input) ─────────────────────────────────────
int N, M;
vector<ll>          skill;
vector<vector<int>> adj;
vector<int>         deg;

// ── Solution struct ───────────────────────────────────────────────────────────
struct Solution {
    vector<bool> inSet;
    vector<int>  conflictCnt;
    ll           score;

    Solution() : inSet(N + 1, false), conflictCnt(N + 1, 0), score(0) {}

    bool canAdd(int u) const { return !inSet[u] && conflictCnt[u] == 0; }

    void add(int u) {
        inSet[u]  = true;
        score    += skill[u];
        for (int v : adj[u]) conflictCnt[v]++;
    }

    void remove(int u) {
        inSet[u]  = false;
        score    -= skill[u];
        for (int v : adj[u]) conflictCnt[v]--;
    }

    // One O(N+M) pass — add every currently free node.
    // Safe to call after any structural change.
    void fillFreeNodes() {
        for (int u = 1; u <= N; u++)
            if (canAdd(u)) add(u);
    }

    vector<int> selected() const {
        vector<int> res;
        for (int i = 1; i <= N; i++)
            if (inSet[i]) res.push_back(i);
        return res;
    }
};

// ── Greedy ────────────────────────────────────────────────────────────────────
Solution greedyBuild(const vector<int>& order) {
    Solution sol;
    for (int u : order)
        if (sol.canAdd(u)) sol.add(u);
    return sol;
}

// mode 0: skill/(deg+1) ratio   mode 1: raw skill   mode 2: degree asc   mode 3: random
vector<int> makeOrder(int mode, mt19937& rng) {
    vector<int> order(N);
    iota(order.begin(), order.end(), 1);
    if (mode == 0) {
        sort(order.begin(), order.end(), [](int a, int b) {
            return (double)skill[a] / (deg[a] + 1) >
                   (double)skill[b] / (deg[b] + 1);
        });
    } else if (mode == 1) {
        sort(order.begin(), order.end(), [](int a, int b) {
            return skill[a] > skill[b];
        });
    } else if (mode == 2) {
        sort(order.begin(), order.end(), [](int a, int b) {
            return deg[a] < deg[b];
        });
    } else {
        shuffle(order.begin(), order.end(), rng);
    }
    return order;
}

// ── Local search: 1-swap ──────────────────────────────────────────────────────
// For each selected node u (cheapest first):
//   - Remove u  →  neighbours may drop to conflictCnt == 0
//   - Greedily add those free neighbours by skill descending
//   - Keep if gained > lost, else revert
// Returns true if at least one improvement was made.
bool oneSwapPass(Solution& sol) {
    bool improved = false;

    vector<int> sel = sol.selected();
    sort(sel.begin(), sel.end(), [](int a, int b) {
        return skill[a] < skill[b];
    });

    for (int u : sel) {
        if (!sol.inSet[u]) continue;

        ll cost = skill[u];
        sol.remove(u);

        vector<int> candidates;
        for (int v : adj[u])
            if (sol.canAdd(v)) candidates.push_back(v);

        sort(candidates.begin(), candidates.end(), [](int a, int b) {
            return skill[a] > skill[b];
        });

        ll gain = 0;
        vector<int> added;
        for (int v : candidates) {
            if (sol.canAdd(v)) {
                sol.add(v);
                added.push_back(v);
                gain += skill[v];
            }
        }

        if (gain > cost) {
            improved = true;
        } else {
            for (int v : added) sol.remove(v);
            sol.add(u);
        }
    }
    return improved;
}

void localSearch(Solution& sol) {
    while (elapsed() < 260) {
        bool imp = oneSwapPass(sol);
        sol.fillFreeNodes();
        if (!imp) break;
    }
}

// ── Iterated Greedy ───────────────────────────────────────────────────────────
// 1-swap gets stuck once no single swap helps — the gap to the true optimum
// may require flipping several nodes simultaneously.
//
// Fix: randomly destroy destroyRate% of the selected set, then rebuild
// greedily. This "kicks" the solution into a new region of the search space,
// after which local search can find a better local optimum.
//
// destroyRate adapts upward when stuck, resets on improvement.
void iteratedGreedy(Solution& best, mt19937& rng) {
    Solution current    = best;
    double destroyRate  = 0.25;
    int    noImprove    = 0;

    auto ratioOrder = makeOrder(0, rng);  // pre-sort once; reused every rebuild

    while (elapsed() < 275) {
        // ── Destroy ──────────────────────────────────────────────────────────
        Solution candidate = current;
        vector<int> sel    = candidate.selected();
        shuffle(sel.begin(), sel.end(), rng);

        int removeCount = max(1, (int)(sel.size() * destroyRate));
        for (int i = 0; i < removeCount; i++)
            candidate.remove(sel[i]);

        // ── Rebuild ───────────────────────────────────────────────────────────
        for (int u : ratioOrder)
            if (candidate.canAdd(u)) candidate.add(u);

        // ── Polish ────────────────────────────────────────────────────────────
        oneSwapPass(candidate);
        candidate.fillFreeNodes();

        // ── Update ────────────────────────────────────────────────────────────
        if (candidate.score > best.score) {
            best        = candidate;
            current     = candidate;
            noImprove   = 0;
            destroyRate = 0.25;          // reset on success
        } else {
            if (candidate.score >= current.score)
                current = candidate;     // accept sideways move
            noImprove++;
        }

        // Widen the destroy window if stuck — explore further away
        if (noImprove > 30) {
            destroyRate = min(0.60, destroyRate + 0.05);
            noImprove   = 0;
            // If already at max destroy rate and still no improvement,
            // the landscape is flat — no point continuing
            if (destroyRate >= 0.60) break;
        }
    }
}

// ── Main ──────────────────────────────────────────────────────────────────────
int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    startTime = chrono::steady_clock::now();

    cin >> N >> M;
    skill.resize(N + 1);
    adj.resize(N + 1);
    deg.resize(N + 1, 0);

    for (int i = 1; i <= N; i++) cin >> skill[i];
    for (int i = 0; i < M; i++) {
        int u, v; cin >> u >> v;
        adj[u].push_back(v);
        adj[v].push_back(u);
        deg[u]++; deg[v]++;
    }

    mt19937 rng(42);
    Solution best;

    // ── Phase 1: Three greedy seeds + local search ────────────────────────────
    for (int mode = 0; mode < 3 && elapsed() < 60; mode++) {
        auto order = makeOrder(mode, rng);
        Solution s = greedyBuild(order);
        s.fillFreeNodes();
        localSearch(s);
        if (s.score > best.score) best = s;
    }

    // ── Phase 2: Iterated Greedy until time limit ─────────────────────────────
    iteratedGreedy(best, rng);

    // ── Output ────────────────────────────────────────────────────────────────
    vector<int> result = best.selected();
    sort(result.begin(), result.end());

    cout << best.score << "\n";
    for (int i = 0; i < (int)result.size(); i++) {
        if (i) cout << " ";
        cout << result[i];
    }
    cout << "\n";

    return 0;
}
