
#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <ostream>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

auto check_numbers(std::string_view line) noexcept -> std::pair<std::int64_t, std::optional<std::int64_t>>
{
    if (!std::isdigit(line[0])) return std::make_pair(1, std::nullopt);
    std::size_t index{1};
    for (; index < line.size(); ++index) {
        if (!std::isdigit(line[index])) break;
    }
    return std::make_pair(index, std::stoll(line.substr(0, index).data()));
}

auto parse_line_numbers(std::string_view current_line) noexcept
{
    std::vector<std::int64_t> numbers;
    std::int64_t index{0};
    while (index < current_line.size()) {
        auto [parsed, number] = check_numbers(current_line.substr(index));
        if (!number.has_value()) {
            ++index;
            continue;
        }
        numbers.push_back(number.value());
        index += parsed;
    }
    return numbers;
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
    std::getline(input_file, line);
    auto race_duration = parse_line_numbers(line.substr(line.find_first_of(':')));
    std::getline(input_file, line);
    auto distance_to_beat = parse_line_numbers(line.substr(line.find_first_of(':')));

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
