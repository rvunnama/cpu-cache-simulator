import matplotlib.pyplot as plt


labels = [
    "No Prefetch",
    "Next-Line Prefetch"
]

hit_rates = [
    0.0,
    50.0
]

memory_reads = [
    10000,
    10000
]


def main():
    plt.figure()

    plt.bar(
        labels,
        hit_rates
    )

    plt.ylabel("Demand Hit Rate (%)")
    plt.title("Next-Line Prefetching on Sequential Workload")

    plt.savefig(
        "results/prefetch_hit_rate.png",
        bbox_inches="tight"
    )

    plt.close()

    plt.figure()

    plt.bar(
        labels,
        memory_reads
    )

    plt.ylabel("Memory Reads")
    plt.title("Memory Traffic with Next-Line Prefetching")

    plt.savefig(
        "results/prefetch_memory_reads.png",
        bbox_inches="tight"
    )

    plt.close()


if __name__ == "__main__":
    main()