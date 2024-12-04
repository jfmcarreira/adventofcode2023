#include <algorithm>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
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
    if (line.empty()) return std::make_pair(0, std::nullopt);
    if (!std::isdigit(line[0])) return std::make_pair(1, std::nullopt);
    std::size_t index{1};
    for (; index < line.size(); ++index) {
        if (!std::isdigit(line[index])) break;
    }
    return std::make_pair(index, std::stoll(line.substr(0, index).data()));
}

struct Operation
{
    std::int64_t lhs;
    std::int64_t rhs;
};

auto parse_operation(std::string_view line) noexcept -> std::pair<std::int64_t, std::optional<Operation>>
{
    constexpr std::string_view kPreffix{"mul("};

    if (line.empty()) return std::make_pair(0, std::nullopt);

    std::int64_t parsed{0};
    if (!line.starts_with("mul(")) return std::make_pair(parsed + 1, std::nullopt);
    parsed += kPreffix.size();
    line = line.substr(parsed);

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

auto parse_line_segment(std::string_view line) noexcept -> std::pair<std::int64_t, std::optional<Operation>>
{
    auto start_index = line.find_first_of('m');
    if (start_index == std::string_view::npos) return std::make_pair(line.size(), std::nullopt);
    auto [parsed, operation] = parse_operation(line.substr(start_index));
    return std::make_pair(start_index + parsed, std::move(operation));
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Missing input!" << std::endl;
        return 1;
    }
    std::ifstream input_file(argv[1], std::ios_base::in);

    std::vector<Operation> operations;

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
