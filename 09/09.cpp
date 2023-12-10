#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <ostream>
#include <ranges>
#include <set>
#include <vector>

inline auto check_numbers(std::string_view line) noexcept -> std::pair<std::int64_t, std::optional<std::int64_t>>
{
    auto first = line[0];
    if (!std::isdigit(first) && first != '-') return std::make_pair(1, std::nullopt);
    std::size_t index{1};
    for (; index < line.size(); ++index) {
        if (!std::isdigit(line[index])) break;
    }
    return std::make_pair(index, std::stoll(line.substr(0, index).data()));
}

inline auto parse_line_numbers(std::string_view current_line) noexcept
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

auto next_value(std::span<const std::int64_t> points) noexcept -> int64_t
{
    auto count = points.size();
    if (count == 1) {
        return points.front();
    }
    if (std::ranges::all_of(points, [](auto n) { return n == 0; })) return 0;

    std::vector<std::int64_t> diff;
    diff.reserve(count);

    std::ranges::transform(
        points.subspan(0, count - 1), points.subspan(1), std::back_inserter(diff), [](auto prev, auto next) {
            return next - prev;
        });

    return points.back() + next_value(diff);
}

auto previous_value(std::span<const std::int64_t> points) noexcept -> int64_t
{
    auto count = points.size();
    if (count == 1) {
        return points.front();
    }
    if (std::ranges::all_of(points, [](auto n) { return n == 0; })) return 0;

    std::vector<std::int64_t> diff;
    diff.reserve(count);

    std::ranges::transform(
        points.subspan(0, count - 1), points.subspan(1), std::back_inserter(diff), [](auto prev, auto next) {
            return next - prev;
        });

    return points.front() - previous_value(diff);
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Missing input!" << std::endl;
        return 1;
    }

    std::ifstream input_file(argv[1], std::ios_base::in);

    std::int64_t result{0};

    std::string line;
    while (std::getline(input_file, line)) {
        auto points = parse_line_numbers(line);
        auto prev = previous_value(points);
        result += prev;
    }
    std::cout << result << std::endl;
    return 0;
}
