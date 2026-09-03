# Process Tracer — Unix Process Monitoring in C

Process Tracer is a client–server process execution and monitoring system written in C. It uses POSIX process primitives, named pipes, and regular pipes to run commands and pipelines, track active executions, persist completed runs, and calculate statistics across selected process IDs.

The project was developed for the Operating Systems course at the University of Minho. It provides a compact practical demonstration of `fork`, `exec`, `wait`, FIFOs, pipes, file descriptors, binary records, and inter-process communication.

## Features

- Execute individual commands
- Execute multi-stage Unix pipelines
- Track the PID, command, start time, and elapsed time of active jobs
- Store completed execution records by PID
- Calculate total execution time across multiple runs
- Count how many times a command appeared in selected executions
- List the unique commands used by selected executions
- Serve concurrent status and statistics requests
- Shut down the monitor through the client
- Build both programs with a single Make command
- No third-party runtime dependencies

## Architecture

```text
                                  ┌──────────────────────────┐
                                  │ Completed run directory  │
                                  │       <data>/<PID>       │
                                  └────────────▲─────────────┘
                                               │
┌────────────────┐     tmp/serverListen     ┌──┴─────────────┐
│     tracer     │ ──────── FIFO ─────────▶ │    monitor     │
│ Client / CLI   │                           │ Server / index │
└───────▲────────┘ ◀──── per-request FIFO ── └──────┬─────────┘
        │                 tmp/<PID>                  │
        │                                            ▼
        │                                  ┌──────────────────┐
        │                                  │ Active-job queue │
        │                                  │    tmp/queue     │
        │                                  └──────────────────┘
        │
        └── fork + execvp ──▶ command or pipeline
```

The `monitor` owns the central FIFO and maintains the execution index. Each `tracer` invocation sends lifecycle events or queries through that FIFO. Query clients create a temporary PID-specific FIFO so the monitor can return their results.

## Requirements

- Linux or another POSIX-compatible operating system
- GCC
- GNU Make

## Build

```bash
git clone https://github.com/guinucool/process-tracer-um.git
cd process-tracer-um
make
```

The build creates:

```text
bin/monitor
bin/tracer
```

Additional Make targets are available:

```bash
make folders server    # Create output directories and build bin/monitor
make folders client    # Create output directories and build bin/tracer
make clean     # Remove objects, binaries, and temporary runtime files
```

## Quick start

### 1. Create the execution-record directory

The monitor stores one binary record per successfully completed execution. Create the destination directory before starting it:

```bash
mkdir -p data
```

### 2. Start the monitor

```bash
./bin/monitor data
```

Keep the monitor running in this terminal. It creates the shared FIFO at `tmp/serverListen` and prints each received message for debugging.

### 3. Run tracer commands

Open another terminal in the repository and use `bin/tracer` to execute programs or query the monitor.

## Usage

### Execute one command

```bash
./bin/tracer execute -u "sleep 2"
```

Example output:

```text
Running PID 12345
Ended in 2001 ms
```

The tracer notifies the monitor when the process starts and when it finishes or fails.

### Execute a pipeline

```bash
./bin/tracer execute -p "cat /etc/passwd | cut -d: -f1 | head -n 5"
```

The tracer creates the necessary child processes and connects their standard input and output using anonymous pipes.

### Show active executions

```bash
./bin/tracer status
```

Each active execution is printed as:

```text
<PID> <command-or-pipeline> <elapsed-time> ms
```

For example:

```text
12345 sleep 1540 ms
12352 cat | cut | head 21 ms
```

### Sum execution times

```bash
./bin/tracer stats-time <PID> [PID ...]
```

Example:

```bash
./bin/tracer stats-time 12345 12352
```

Output:

```text
Total execution time is 2022 ms
```

### Count command executions

```bash
./bin/tracer stats-command <command> <PID> [PID ...]
```

Example:

```bash
./bin/tracer stats-command sleep 12345 12360 12372
```

Output:

```text
sleep was executed 3 times
```

For pipeline records, each executable in the pipeline is counted independently.

### List unique commands

```bash
./bin/tracer stats-uniq <PID> [PID ...]
```

Example:

```bash
./bin/tracer stats-uniq 12345 12352
```

Example output:

```text
sleep
cat
cut
head
```

### Stop the monitor

```bash
./bin/tracer shutdown
```

## Command reference

| Command | Purpose |
| --- | --- |
| `tracer execute -u "<command>"` | Execute and trace one command |
| `tracer execute -p "<pipeline>"` | Execute and trace a pipeline |
| `tracer status` | List executions currently in progress |
| `tracer stats-time <PID...>` | Sum the duration of selected executions |
| `tracer stats-command <command> <PID...>` | Count a command across selected executions |
| `tracer stats-uniq <PID...>` | List unique commands from selected executions |
| `tracer shutdown` | Request a clean monitor shutdown |

> [!NOTE]
> Command strings are parsed and passed directly to `execvp`; they are not interpreted by a shell. Shell features such as redirection, variable expansion, globbing, command substitution, and quoted arguments inside the command string are therefore not expanded. Pipelines are supported explicitly through `execute -p`.

## How it works

### Executing a process

1. `tracer` opens the monitor's FIFO.
2. It parses the requested command or pipeline.
3. A child process sends a start message containing its PID, command, and timestamp.
4. The child executes the program with `execvp`, or creates the pipeline processes and pipes.
5. The parent waits for completion and measures elapsed wall-clock time.
6. It sends either a successful-completion or failure message to the monitor.
7. Successful executions are persisted under the configured data directory using their PID as the filename.

### Monitoring active jobs

The monitor appends start records to `tmp/queue`. Completion messages mark the corresponding queue entries as finished. A `status` request scans the queue and returns only unfinished records, calculating their current elapsed time.

### Serving statistics

For each statistics request, the monitor forks a worker, reads the requested per-PID records, performs the aggregation, and sends replies through a temporary FIFO belonging to the requesting tracer process.

## IPC message types

| Type | Direction | Meaning |
| :---: | --- | --- |
| `P` | Tracer → Monitor | Process started |
| `E` | Tracer → Monitor | Process completed successfully |
| `F` | Tracer → Monitor | Process failed |
| `S` | Tracer → Monitor | Active-status request |
| `T` | Tracer → Monitor | Total-time request |
| `C` | Tracer → Monitor | Command-count request |
| `U` | Tracer → Monitor | Unique-command request |
| `R` | Monitor → Tracer | Query reply |
| `W` | Tracer → Monitor | Shutdown request |

## Project structure

```text
.
├── Makefile
├── includes/
│   ├── utils.h
│   └── tools/
│       ├── dynarray.h       # Dynamic string-array operations
│       ├── message.h        # FIFO message format and request API
│       ├── process.h        # Process execution and active queue
│       └── register.h       # Persistent records and statistics
└── src/
    ├── monitor.c            # Monitor entry point
    ├── tracer.c             # Tracer CLI entry point
    ├── utils.c              # Timing, errors, and argument helpers
    └── tools/
        ├── dynarray.c       # Command and pipeline parsing
        ├── message.c        # FIFO messaging and reply formatting
        ├── process.c        # fork/exec, pipelines, and active jobs
        └── register.c       # Per-PID storage and aggregations
```

## Runtime files

| Path | Contents |
| --- | --- |
| `tmp/serverListen` | Main client-to-monitor FIFO |
| `tmp/queue` | Binary active/execution queue |
| `tmp/<PID>` | Temporary FIFO for one query client |
| `<data-directory>/<PID>` | Persisted successful execution record |

## Concepts demonstrated

- Parent and child process creation with `fork`
- Program replacement with `execvp`
- Process synchronization with `wait`
- Anonymous pipes for pipeline construction
- Named pipes for client–server IPC
- File-descriptor redirection with `dup2`
- Binary file storage with `read`, `write`, `open`, and `lseek`
- Dynamic memory and string-array management
- Wall-clock execution-time measurement
- Concurrent request handling through worker processes

## License

No license is currently specified. Unless a license is added, the source code remains protected by standard copyright rules.
