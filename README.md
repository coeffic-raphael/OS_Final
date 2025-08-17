# OS_Final — Concurrent MST Server (C++17)

A multi-threaded TCP server that builds **per-client weighted graphs** and computes a **Minimum Spanning Tree (MST)** using **Prim** or **Kruskal**.  
It’s a compact lab for **concurrency patterns** + **graph algorithms**, with tooling for **Valgrind** (Memcheck/Helgrind/Callgrind) and **gcov** coverage.

---

## 📦 Project Layout

- `ActiveObject.(hpp|cpp)` — Active Object (worker thread + queue of `(Message<T>, func)`).
- `CommandHandler.(hpp|cpp)` — Parses commands; orchestrates **Pipeline** (AOs) or **Leader–Follower** (ThreadPool).
- `Graph.(hpp)` / `GraphBase.(hpp)` — Undirected weighted graph (adjacency list).
- `Prim.(hpp)` / `Kruskal.(hpp)` — MST algorithms (**Strategy** implementations).
- `MSTAlgos.(hpp)` — Abstract MST interface (`solveMST`).
- `MSTFactory.(hpp)` — **Factory**: chooses `Prim`/`Kruskal` by string.
- `ThreadPool.(hpp|cpp)` — Task queue with **Leader–Follower** hand-off.
- `Tree.(hpp)` — MST result + metrics (total weight, diameter, avg, min edge).
- `Message.(hpp)` — Shared message/context passed between stages.
- `Server.(hpp|cpp)` — TCP server; per-client thread; graceful stop.
- `main.cpp` — Entry point (port).
- `Makefile` — Build, run, coverage, Valgrind, profiling targets.
- `Coverage/` — generated gcov reports (`*.gcov`, `coverage.txt`).
- `Valgrinds/` — generated analyses (`valgrind_memcheck.txt`, `valgrind_helgrind.txt`, `callgrind.out`, `callgrind_summary.txt`).

---

## 🧩 Design & Patterns

- **Active Object (AO)**: `ActiveObject<T>` runs a worker thread consuming tasks.
  - In `CommandHandler<T>` we wire a **3-stage pipeline**:
    1. `parserAO` → parse & validate commands; manage the client’s `Graph<T>`.
    2. `mstAO` → compute MST via `MSTFactory<T>::createSolver("prim"|"kruskal")`.
    3. `metricsAO` → compute metrics from the `Tree<T>`: total, diameter (two BFS), average pairwise distance, shortest edge.
- **Pipeline**: `MST <algo> Pipeline` pushes the same `Message<T>` through AO stages in order.
- **Leader–Follower (LF)**: `MST <algo> LF` enqueues two tasks to `ThreadPool`. Only one worker is **leader** at a time; after executing a task, leadership is passed to the next waiting thread, reducing contention.
- **Strategy + Factory**: algorithm chosen at runtime; clean, testable separation.

---

## 🧮 Algorithms

- **Prim**: priority-queue variant; supports **disconnected** graphs by starting from every unvisited vertex (forest).
- **Kruskal**: sorts edges; **Union–Find** with path compression + union-by-rank; avoids duplicates by taking `u < v` for undirected edges.
- **Metrics on `Tree<T>`**:
  - **Total weight** — sum of edge weights.
  - **Diameter** — BFS from arbitrary node → farthest A; BFS from A → max distance.
  - **Average distance** — BFS from every vertex; average unique pairs.
  - **Shortest distance** — min edge weight in the tree.

---

## 🔌 Text Protocol (per TCP client)

Each TCP client has its **own** graph state (keyed by its file descriptor). Commands are line-based:

- `Newgraph <V>`  
  Create/overwrite a graph with vertices `0..V-1`.
- `Newedge <u> <v> <w>`  
  Add undirected edge `(u,v)` with weight `w` (project instantiates `T = double`).
- `MST <algo> <pattern>`  
  - `<algo>`: `Prim` | `Kruskal` (case-insensitive)
  - `<pattern>`: `Pipeline` | `LF`  
  Returns metrics for the MST.
- `exit`  
  Server replies `exit` and flips a global atomic to **stop gracefully**.

Errors are returned as: `Error: <message>`.

---

## 🛠 Build & Run

**Requirements**: Linux, `g++` (C++17), POSIX sockets, pthreads.

~~~bash
# Build (debug, warnings, coverage flags):
make

# Run server (default 1111; or pass a port)
./server
./server 5555
~~~

### Quick test with netcat

~~~bash
# Terminal 1
./server 1111

# Terminal 2
nc 127.0.0.1 1111
Newgraph 5
Newedge 0 1 2.5
Newedge 1 2 1.2
Newedge 0 3 4.0
Newedge 3 4 0.7
MST Prim Pipeline
MST Kruskal LF
exit
~~~

**Typical MST response:**
MST successfully calculated with Pipeline.
Total weight: 4.4
Longest distance: 5.2
Average distance: 2.1
Shortest distance: 0.7


---

## 🧪 Tooling Targets (Makefile)

~~~bash
make clean        # remove objects, binary, coverage & valgrind outputs
make run          # ./server (shortcut)

make coverage     # run server once (interactive) and write gcov into Coverage/ and coverage.txt

make memcheck     # Valgrind Memcheck -> Valgrinds/valgrind_memcheck.txt
make helgrind     # Valgrind Helgrind (race detection) -> Valgrinds/valgrind_helgrind.txt

make callgrind    # Valgrind Callgrind -> Valgrinds/callgrind.out + callgrind_summary.txt
make kcachegrind  # visualize Callgrind (requires GUI)
~~~

> Build flags: `-std=c++17 -pthread -Wall -Wextra -g -O0 -fprofile-arcs -ftest-coverage` (links `-lgcov`).

---

## 🔒 Concurrency & Safety

- **Per-client threads** in `Server`; all joined on `stop()`.
- **Global atomic** `serverRunning` shared across components for coordinated shutdown.
- `CommandHandler` guards `clientGraphs` with a **mutex**.
- Each `ActiveObject` owns one worker thread and a private queue guarded by mutex + condvar.
- `ThreadPool` uses mutex/condvar, a guarded task queue, and a `leaderAssigned` flag for LF rotation.

---

## 🧭 Extending

- Add an MST: implement `MSTAlgos<T>` and register it in `MSTFactory<T>::createSolver`.
- Add a command: parse in `CommandHandler::handleCommand` and wire to **Pipeline** (AOs) or **LF** (ThreadPool).
- Change numeric type: add explicit template instantiation for `float`/`long double` if needed.

---

## ❗ Notes & Limitations

- Single-process demo server; not hardened for untrusted input.
- Protocol is minimal (no framing beyond line endings).
- For reproducible coverage, script a client that feeds commands non-interactively.

---

## ✅ Summary

This repo demonstrates:
- Real **concurrency patterns** (Active Object, Pipeline, Leader–Follower)  
- Solid **MST algorithms** (Prim, Kruskal) behind a **Factory/Strategy** design  
- Practical ops tooling (**gcov**, **Valgrind** suite)  
- Clean per-client state handling and graceful shutdown

Happy hacking! 🧵🕸️🌲

