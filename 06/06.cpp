
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <ostream>
#include <ranges>
#include <vector>

#include "../common/common.hpp"

auto parse_line_single_number(std::string line) noexcept
{
    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
    return parse_line_numbers(line);
}

auto calculate_distance(std::int64_t load_time, std::int64_t duration)
{
    return (duration - load_time) * load_time;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Missing input!" << std::endl;
        return 1;
    }

    std::ifstream input_file(argv[1], std::ios_base::in);

    std::string line;

    // Part One use parse_line_numbers
    std::getline(input_file, line);
    auto race_duration = parse_line_single_number(line.substr(line.find_first_of(':')));
    std::getline(input_file, line);
    auto distance_to_beat = parse_line_single_number(line.substr(line.find_first_of(':')));

    std::int64_t result_part_one{1};
    for (std::size_t i = 0; i < race_duration.size(); ++i) {

        auto time = race_duration[i];
        auto dist = distance_to_beat[i];
        auto min = *std::ranges::lower_bound(
            std::ranges::views::iota(0, time), dist, std::less_equal<>(), [&](const auto& load) {
                return calculate_distance(load, time);
            });

        auto max = *std::ranges::lower_bound(
            std::ranges::views::iota(min + 1, time), dist, std::greater<>(), [&](const auto& load) {
                return calculate_distance(load, time);
            });

        auto win_count = max - min;
        result_part_one *= win_count;
    }
    std::cout << result_part_one << std::endl;
    return 0;
}
