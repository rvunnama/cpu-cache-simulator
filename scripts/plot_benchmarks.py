import csv
import os
import sys

import matplotlib.pyplot as plt


def load_results(path):
    rows = []

    with open(path, newline="") as file:
        reader = csv.DictReader(file)

        for row in reader:
            rows.append(row)

    return rows


def select_configuration(rows):
    """
    Keep one consistent policy combination so the graph
    primarily demonstrates cache-size behavior.
    """

    selected = []

    for row in rows:
        if (
            row["ReplacementPolicy"] == "LRU"
            and row["WritePolicy"] == "Write-through"
            and row["WriteMissPolicy"] == "Write-allocate"
            and int(row["Associativity"]) == 2
            and int(row["BlockSize"]) == 16
        ):
            selected.append(row)

    selected.sort(
        key=lambda row: int(row["CacheSize"])
    )

    return selected


def plot_hit_rate(rows):
    cache_sizes = [
        int(row["CacheSize"])
        for row in rows
    ]

    hit_rates = [
        float(row["HitRate"])
        for row in rows
    ]

    plt.figure()

    plt.plot(
        cache_sizes,
        hit_rates,
        marker="o"
    )

    plt.xlabel("Cache Size (bytes)")
    plt.ylabel("Hit Rate (%)")
    plt.title("Hit Rate vs. Cache Size")
    plt.grid(True)

    os.makedirs("results", exist_ok=True)

    plt.savefig(
        "results/hit_rate_vs_cache_size.png",
        bbox_inches="tight"
    )

    plt.close()


def plot_average_cost(rows):
    cache_sizes = [
        int(row["CacheSize"])
        for row in rows
    ]

    costs = [
        float(row["AverageAccessCost"])
        for row in rows
    ]

    plt.figure()

    plt.plot(
        cache_sizes,
        costs,
        marker="o"
    )

    plt.xlabel("Cache Size (bytes)")
    plt.ylabel("Average Access Cost (ns)")
    plt.title(
        "Average Access Cost vs. Cache Size"
    )

    plt.grid(True)

    plt.savefig(
        "results/access_cost_vs_cache_size.png",
        bbox_inches="tight"
    )

    plt.close()


def main():
    if len(sys.argv) != 2:
        print(
            "Usage: python3 scripts/plot_benchmarks.py "
            "<benchmark.csv>"
        )
        return 1

    rows = load_results(sys.argv[1])

    selected = select_configuration(rows)

    if not selected:
        print(
            "No matching benchmark configurations found."
        )
        return 1

    plot_hit_rate(selected)
    plot_average_cost(selected)

    print(
        "Generated benchmark graphs in results/"
    )

    return 0


if __name__ == "__main__":
    raise SystemExit(main())