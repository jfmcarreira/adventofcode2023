#include <__algorithm/fold.h>

#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <span>
#include <string>
#include <vector>

template<typename F>
auto parse_file(const std::filesystem::path& file_name, F&& parsing) noexcept
{
    std::ifstream input_file(file_name, std::ios_base::in);
    std::string line;
    while (std::getline(input_file, line)) {
        parsing(line);
    }
}

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

struct Report
{
    std::vector<std::int64_t> levels;
};

auto validate(const Report& report) noexcept
{
    auto is_increasing = std::ranges::is_sorted(report.levels, std::greater<>{});
    auto is_decreasing = std::ranges::is_sorted(report.levels, std::less<>{});
    if (!is_increasing && !is_decreasing) return false;

    for (auto i = 1ULL; i < report.levels.size(); ++i) {
        auto diff = std::abs(report.levels[i - 1] - report.levels[i]);
        if (diff < 1 || diff > 3) return false;
    }
    return true;
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
