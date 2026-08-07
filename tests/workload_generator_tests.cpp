#include "WorkloadGenerator.hpp"

#include <cassert>
#include <iostream>

int main() {
    const auto sequential =
        WorkloadGenerator::generate(
            "sequential",
            4,
            0,
            16,
            4,
            42
        );

    assert(sequential.size() == 4);
    assert(sequential[0].address == 0);
    assert(sequential[1].address == 16);
    assert(sequential[2].address == 32);
    assert(sequential[3].address == 48);

    const auto loop =
        WorkloadGenerator::generate(
            "loop",
            6,
            0,
            16,
            2,
            42
        );

    assert(loop[0].address == 0);
    assert(loop[1].address == 16);
    assert(loop[2].address == 0);
    assert(loop[3].address == 16);

    const auto randomFirst =
        WorkloadGenerator::generate(
            "random",
            100,
            0,
            16,
            8,
            42
        );

    const auto randomSecond =
        WorkloadGenerator::generate(
            "random",
            100,
            0,
            16,
            8,
            42
        );

    assert(
        randomFirst.size() ==
        randomSecond.size()
    );

    for (
        std::size_t index = 0;
        index < randomFirst.size();
        ++index
    ) {
        assert(
            randomFirst[index].address ==
            randomSecond[index].address
        );
    }

    std::cout
        << "All workload generator tests passed.\n";

    return 0;
}