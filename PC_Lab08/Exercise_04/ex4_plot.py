"""Exercise 4: Time vs processors and Speedup graphs.
Usage: python3 ex4_plot.py            (reads ex2_results.csv and ex3_results.csv)
CSV format: procs,time  (repeat runs allowed - they are averaged).
Needs: pip install matplotlib
"""
import csv, collections
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

def load(path):
    d = collections.defaultdict(list)
    with open(path) as f:
        for row in csv.DictReader(f):
            d[int(row["procs"])].append(float(row["time"]))
    procs = sorted(d)
    return procs, [sum(d[p]) / len(d[p]) for p in procs]

programs = [("ex2_results.csv", "Exercise 2: Sum 1..10,000,000"),
            ("ex3_results.csv", "Exercise 3: Monte Carlo Pi (10,000,000)")]
data = []
for path, title in programs:
    try:
        p, t = load(path)
        if p:
            data.append((title, p, t))
    except FileNotFoundError:
        pass
if not data:
    raise SystemExit("No data found - fill ex2_results.csv / ex3_results.csv first.")

# Time vs processors
plt.figure(figsize=(7, 5))
for title, p, t in data:
    plt.plot(p, t, "o-", label=title)
plt.xscale("log", base=2); plt.yscale("log")
plt.xlabel("Number of processors"); plt.ylabel("Time (s)")
plt.title("Time vs Number of Processors"); plt.grid(True, which="both", alpha=.3); plt.legend()
plt.savefig("ex4_time_vs_procs.png", dpi=150, bbox_inches="tight")

# Speedup = T(1)/T(p)
plt.figure(figsize=(7, 5))
maxp = 1
for title, p, t in data:
    if 1 not in p:
        print("Warning: no 1-process run for", title, "- speedup needs T(1)"); continue
    t1 = t[p.index(1)]
    plt.plot(p, [t1 / x for x in t], "o-", label=title)
    maxp = max(maxp, max(p))
ideal = [1]
while ideal[-1] * 2 <= maxp: ideal.append(ideal[-1] * 2)
plt.plot(ideal, ideal, "k--", label="Ideal (speedup = p)")
plt.xscale("log", base=2); plt.yscale("log", base=2)
plt.xlabel("Number of processors"); plt.ylabel("Speedup  T(1)/T(p)")
plt.title("Speedup"); plt.grid(True, which="both", alpha=.3); plt.legend()
plt.savefig("ex4_speedup.png", dpi=150, bbox_inches="tight")
print("Saved ex4_time_vs_procs.png and ex4_speedup.png")
