# Parallel Matrix Multiplication System

> MSCS 632 – Advanced Programming Languages  
> Data Processing System Assignment — **Java + Go Implementations**

This repository contains two parallel matrix multiplication variants:

- **Java**: producer-consumer with a bounded shared queue, thread-safe result accumulation, and `ExecutorService` worker threads.
- **Go**: goroutines + buffered channels for task distribution, shared result storage with mutex protection, and idiomatic error/verification handling.

Both implementations compute **C = A × B** in parallel and verify the result against a serial multiplication.

---

## How It Works

### Java version

```
Main Thread (Producer)
       │
       │  addTask(row_i)          ← bounded, synchronized queue
       ▼
 SharedTaskQueue  ◄──► wait / notifyAll
       │
       │  getTask()
       ▼
 Worker Threads × 4  (ExecutorService / FixedThreadPool)
       │
       │  storeRow(i, result)
       ▼
  ResultStore  ──►  result_matrix.txt
```

The Java implementation schedules one row task per worker thread, computes row-wise dot products for **C = A × B**, stores results in a thread-safe matrix, and compares the parallel output with a serial reference result.

### Go version

```
main goroutine (producer)
       │
       │  send MatrixTask  ← buffered channel
       ▼
    taskCh (buffered)
       │
       │  receive task
       ▼
 Worker goroutines × 4
       │
       │  computeRow()
       │  storeRow() with mutex
       ▼
  ResultStore  ──►  result_matrix.txt
```

The Go implementation uses a buffered channel as the shared task queue, closes the channel as a completion signal, and collects worker summaries through a stats channel.

---

## Project Structure

```
java/
├── src/
│   ├── MatrixMultiplicationSystem.java  ← main entry point / orchestrator
│   ├── MatrixTask.java                  ← immutable task (row data)
│   ├── SharedTaskQueue.java             ← thread-safe bounded queue
│   ├── MatrixWorker.java                ← Runnable worker thread
│   └── ResultStore.java                 ← thread-safe result accumulator
├── run.bat                              ← Windows launcher

Go/
├── src/
│   ├── main.go
│   ├── matrix_task.go
│   ├── result_store.go
│   ├── worker.go
│   └── errors.go
├── run.bat
├── result_matrix.txt
```

---

## Requirements

| Requirement | Version |
|-------------|---------|
| Java (JDK)  | 11 or higher |
| Go          | 1.21 or higher |

**Install tools if needed:**

| OS | Java | Go |
|----|------|----|
| Windows | Download from [Adoptium](https://adoptium.net/) | Download from [go.dev](https://go.dev/dl/) |
| macOS | `brew install openjdk` | `brew install go` |
| Ubuntu/Debian | `sudo apt install default-jdk` | `sudo apt install golang-go` |

Verify your installs:

```bash
java -version
go version
```

---

## Running the Programs

### Java (Windows)

```bat
cd java
run.bat
```

### Java (manual)

```bash
cd java
mkdir -p out
javac -d out src/*.java
java -cp out matrix.MatrixMultiplicationSystem
```

### Go (Windows)

```bat
cd Go
run.bat
```

### Go (manual)

```bash
cd Go/src
go build -o ../matrixmul.exe .
cd ..
./matrixmul.exe
```

Both implementations write output to `result_matrix.txt` in their respective directories.

---

## Sample Output

Java and Go outputs include startup banners, worker progress summaries, verification status, and a final result file.

Example Java log excerpts:

```text
INFO    Matrix A: 8×6   Matrix B: 6×7   Workers: 4
INFO    Worker-1: 2 tasks processed, 0 error(s)
INFO    Total rows completed: 8 / 8  |  Errors: 0  |  Time: 77ms
INFO    Verification vs serial result: ✓ CORRECT
INFO    Result written to result_matrix.txt
```

Example Go log excerpts:

```text
2026/04/12 10:22:01.993 ====================================================
2026/04/12 10:22:01.993   Parallel Matrix Multiplication System -- Go
2026/04/12 10:22:01.993 Matrix A: 8x6   Matrix B: 6x7   Workers: 4
2026/04/12 10:22:01.993 --- Enqueueing tasks ---
2026/04/12 10:22:01.993 Enqueued task 0
2026/04/12 10:22:01.993 --- All workers done | rows: 8/8 | errors: 0 | time: 15ms ---
2026/04/12 10:22:01.994 Verification vs serial result: CORRECT
2026/04/12 10:22:01.994 Result written to "result_matrix.txt"
```

---

## Concurrency Techniques Used

### Java

| Technique | Where | Purpose |
|-----------|-------|---------|
| `synchronized` blocks | `SharedTaskQueue`, `ResultStore` | Protect shared state |
| `wait()` / `notifyAll()` | `SharedTaskQueue` | Block and wake threads safely |
| `ExecutorService` | Main class | Manage worker thread pool |
| Poison-pill (`markDone()`) | `SharedTaskQueue` | Signal workers to complete |

### Go

| Technique | Where | Purpose |
|-----------|-------|---------|
| buffered channels | `taskCh` | Task queue with built-in blocking |
| channel close | `taskCh` | Signal completion to workers |
| `sync.Mutex` | `ResultStore` | Protect shared result matrix |
| stats channel | `statsCh` | Worker completion reporting |

---

## Error Handling

### Java

| Exception | Where | Strategy |
|-----------|-------|----------|
| `InterruptedException` | Worker, Queue | Restore interrupt flag and exit |
| `IOException` | `ResultStore` | Log and rethrow |
| Runtime task errors | `MatrixWorker` | Log and continue |

### Go

| Error | Where | Strategy |
|------|-------|----------|
| channel send failures | `taskCh` | avoided by closing only after all sends |
| worker errors | `RunWorker` | sent through `errCh` and logged after workers finish |
| file write errors | `WriteToFile()` | returned and logged in `main()` |

---

## Configuration

### Java

Edit constants in `java/src/MatrixMultiplicationSystem.java`:

```java
private static final int ROWS_A      = 8;
private static final int INNER       = 6;
private static final int COLS_B      = 7;
private static final int NUM_WORKERS = 4;
private static final int QUEUE_CAP   = 16;
```

### Go

Edit constants in `Go/src/main.go`:

```go
const (
    rowsA      = 8
    inner      = 6
    colsB      = 7
    numWorkers = 4
    queueCap   = 16
    outFile    = "result_matrix.txt"
)
```

---

## Notes

- Both directories produce their own `result_matrix.txt` file.
- The Go `run.bat` builds `matrixmul.exe` and removes it after execution.
- The Java `run.bat` compiles and runs using the source files under `java/src`.
