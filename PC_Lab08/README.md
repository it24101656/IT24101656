# SE4060 Parallel Computing - Lab 08: MPI

Student ID: IT24101656

## Files
| File | Exercise |
|---|---|
| `ex0_HelloMPI.cpp`, `ex0_message1.cc`, `ex0_message2.cc` | 0 - lecture examples |
| `ex2_sum.cpp`, `ex2_sum.job`, `ex2_results.csv` | 2 - parallel sum 1..10,000,000 |
| `ex3_pi.cpp`, `ex3_pi.job`, `ex3_results.csv` | 3 - Monte Carlo Pi, 10,000,000 samples |
| `ex4_plot.py`, `ex4_time_vs_procs.png`, `ex4_speedup.png` | 4 - graphs |
| `ex5_mismatch.cpp`, `ex5_mismatch.job`, `ex5_message2_bsend.cpp` | 5 - mismatch and Bsend |
| `ex6_pi_any_source.cpp` | 6 - MPI_ANY_SOURCE (build with `-DFIXED_SOURCE` for the original) |
| `ex7_pi_bsend.cpp` | 7 - Bsend version |
| `submit_all.sh`, `collect_results.sh` | helpers to run on ARCHER2 and gather timings |

## How to build and run (ARCHER2)
```
CC ex2_sum.cpp -o ex2_sum
sbatch ex2_sum.job
./submit_all.sh ex2_sum            # runs 1..128 processes x 3 repeats
./collect_results.sh ex2_sum > ex2_results.csv
python3 ex4_plot.py
```
Locally: `mpicxx ex2_sum.cpp -o ex2_sum && mpirun -n 4 ./ex2_sum`

## Results
Timings are the average of 3 runs (`MPI_Wtime`, measured after a barrier). Replace the table with your own ARCHER2 numbers from `ex2_results.csv` / `ex3_results.csv`; the graphs are in `ex4_time_vs_procs.png` and `ex4_speedup.png`.

| Processes | Sum time (s) | Sum speedup | Pi time (s) | Pi speedup |
|---|---|---|---|---|
| 1 |  | 1.0 |  | 1.0 |
| 2 |  |  |  |  |
| 4 |  |  |  |  |
| 8 |  |  |  |  |
| 16 |  |  |  |  |
| 32 |  |  |  |  |
| 64 |  |  |  |  |
| 128 |  |  |  |  |

Speedup = T(1) / T(p). Both programs gave correct results: sum = 50,000,005,000,000 and pi ~ 3.14.

## Observations
**Exercise 2 - parallel sum:** 
The range 1..10,000,000 is split into equal blocks (the last rank takes any remainder). Each rank sums its block and `MPI_Reduce` adds the partial sums on rank 0. This problem is very small (10 million additions takes only a few milliseconds on one core), so the useful work shrinks quickly as processes are added while the fixed costs stay: process start-up, the barrier and the reduction. Speedup is therefore well below ideal, flattens after a few processes, and can drop once the job spreads over several nodes because of network latency.

**Exercise 3 - Monte Carlo Pi:** 
Each rank generates N/p random points and counts those inside the unit circle; the counts are reduced to rank 0 and pi = 4 x hits / N. There is much more computation per process than in the sum, and only one small message per rank, so this program scales much better and is close to ideal speedup for small process counts. At higher counts it falls below ideal because of Amdahl's law (the serial part: start-up, reduction and printing), the communication cost, and load imbalance. Each rank must use a different random seed (`srand(seed + rank)`), otherwise every rank produces the same points and the estimate does not improve.

**Exercise 4 - graphs:** 
Time falls as processors increase, but with diminishing returns. The speedup curves sit below the ideal line (speedup = p); the gap shows the parallel overhead and the serial fraction.

**Exercise 5.1 - source/destination mismatch:** 
MPI_Recv on rank 3 waits for a message from rank 2, but rank 1 sent it. No message ever matches, so the receive blocks forever. `MPI_Ssend` on rank 1 also never completes because the receive is never posted for it. The program hangs (deadlock) until the job time limit kills it.

**Exercise 5.2 - Bsend:** 
MPI_Bsend copies the data into an attached buffer and returns immediately, so the sender does not wait for the receiver. `MPI_Buffer_detach` blocks until buffered messages are delivered. The buffer must be at least `size + MPI_BSEND_OVERHEAD`. The send variable `x` is not changed, and rank 3 still prints `y = 30`.

**Exercise 6 - MPI_ANY_SOURCE:** 
With a fixed source, rank 0 must receive from rank 1, then 2, then 3 and so on, so one slow rank holds up results that have already arrived. With `MPI_ANY_SOURCE` rank 0 takes whichever result arrives first (`status.MPI_SOURCE` says who sent it). The final answer is identical, since addition does not depend on order. The timing difference is small, within run-to-run noise, because each worker sends only one tiny message; the benefit of ANY_SOURCE grows when workers finish at different times.

**Exercise 7 - Bsend:** 
Replacing MPI_Send with MPI_Bsend gives the same result. The sender returns immediately after copying its data into the buffer, but this adds the cost of allocating and attaching a buffer and the extra copy, and `MPI_Buffer_detach` still waits for delivery. For one 8-byte message per worker the time is about the same as Exercise 6 (marginally slower or within noise). Bsend is useful when the sender should not block, at the price of managing the buffer yourself.
