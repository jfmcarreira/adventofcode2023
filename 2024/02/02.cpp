#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "../common.hpp"

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

struct Report
{
    std::vector<std::int64_t> levels;
};

auto validate(const std::vector<std::int64_t>& levels) noexcept
{
    auto is_increasing = std::ranges::is_sorted(levels, std::greater<>{});
    auto is_decreasing = std::ranges::is_sorted(levels, std::less<>{});
    if (!is_increasing && !is_decreasing) return false;

    for (auto i = 1ULL; i < levels.size(); ++i) {
        auto diff = std::abs(levels[i - 1] - levels[i]);
        if (diff < 1 || diff > 3) return false;
    }
    return true;
}

auto copy_unless(
    const std::vector<std::int64_t>& source,
    std::size_t discard_index,
    std::vector<std::int64_t>& destination) noexcept
{
    for (auto i = 0; i < source.size(); ++i) {
        if (i == discard_index) continue;
        destination.push_back(source[i]);
    }
}

auto validate(const Report& report) noexcept
{
    if (validate(report.levels)) return true;
    std::vector<std::int64_t> levels;
    levels.reserve(report.levels.size() - 1);
    for (auto i = 0; i < report.levels.size(); ++i) {
        levels.clear();
        copy_unless(report.levels, i, levels);
        if (validate(levels)) return true;
    }
    return false;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Missing input!" << std::endl;
        return 1;
    }
    std::ifstream input_file(argv[1], std::ios_base::in);

    std::vector<Report> reports;

    auto parsing = [&](const std::string& line) { reports.push_back({parse_line_numbers(line)}); };
    parse_file(argv[1], parsing);

    auto result = std::ranges::fold_left(
        reports, 0, [](const auto& sum, const auto& report) { return sum + (validate(report) ? 1 : 0); });

    std::cout << result << std::endl;
    return 0;
}
