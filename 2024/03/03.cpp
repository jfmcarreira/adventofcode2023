#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "../common.hpp"

constexpr std::string_view kOperation{"mul"};
constexpr std::string_view kEnable{"do()"};
constexpr std::string_view kDisable{"don't()"};

struct Operation
{
    std::int64_t lhs;
    std::int64_t rhs;
};

auto parse_operation(std::string_view line) noexcept -> std::pair<std::int64_t, std::optional<Operation>>
{
    if (line.empty()) return std::make_pair(0, std::nullopt);

    std::int64_t parsed{0};
    if (!line.starts_with(kOperation)) return std::make_pair(parsed + 1, std::nullopt);
    parsed += kOperation.size();
    line = line.substr(parsed);

    parsed += 1;
    if (line[0] != '(') return std::make_pair(parsed, std::nullopt);
    line = line.substr(1);

    Operation result;

    {
        auto [count, number] = check_numbers(line);
        parsed += count;
        if (!number.has_value()) return std::make_pair(parsed, std::nullopt);
        line = line.substr(count);

        result.lhs = number.value();
    }

    parsed += 1;
    if (line[0] != ',') return std::make_pair(parsed, std::nullopt);
    line = line.substr(1);

    {
        auto [count, number] = check_numbers(line);
        parsed += count;
        if (!number.has_value()) return std::make_pair(parsed, std::nullopt);
        line = line.substr(count);

        result.rhs = number.value();
    }

    parsed += 1;
    if (line[0] != ')') return std::make_pair(parsed, std::nullopt);

    return std::make_pair(parsed, result);
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Missing input!" << std::endl;
        return 1;
    }
    std::ifstream input_file(argv[1], std::ios_base::in);

    std::vector<Operation> operations;
    bool enabled{true};

    auto parse_line_segment =
        [&enabled](std::string_view line) noexcept -> std::pair<std::int64_t, std::optional<Operation>> {
        auto start_index = line.find_first_of("md");
        if (start_index == std::string_view::npos) return std::make_pair(line.size(), std::nullopt);

        auto segment = line.substr(start_index);
        if (enabled && segment[0] == 'm') {
            auto [parsed, operation] = parse_operation(segment);
            return std::make_pair(start_index + parsed, std::move(operation));
        }

        if (segment.starts_with(kDisable)) {
            enabled = false;
            return std::make_pair(start_index + kDisable.size(), std::nullopt);
        }

        if (segment.starts_with(kEnable)) {
            enabled = true;
            return std::make_pair(start_index + kEnable.size(), std::nullopt);
        }

        return std::make_pair(start_index + 1, std::nullopt);
    };

    auto parsing = [&](const std::string& line) {
        std::int64_t index{0};

        while (index < line.size()) {
            auto [parsed, operation] = parse_line_segment(line.substr(index));
            if (operation.has_value()) {
                operations.push_back(operation.value());
            }
            index += parsed;
        }
    };
    parse_file(argv[1], parsing);

    auto result =
        std::ranges::fold_left(operations, 0, [](const auto& sum, const auto& op) { return sum + op.lhs * op.rhs; });

    std::cout << result << std::endl;
    return 0;
}
