#include <__algorithm/fold.h>

#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <ostream>
#include <ranges>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
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

struct Comand
{
    std::int64_t result{0};
    std::vector<std::int64_t> operands;
};

using Operation = std::function<std::int64_t(std::int64_t, std::int64_t)>;

const std::array<Operation, 2> kOperations{
    [](std::int64_t lhs, std::int64_t rhs) noexcept { return lhs + rhs; },
    [](std::int64_t lhs, std::int64_t rhs) noexcept { return lhs * rhs; },
};

auto check_number(std::string_view input) noexcept -> std::pair<std::int64_t, std::optional<std::int64_t>>
{
    if (input.empty()) return std::make_pair(0, std::nullopt);
    if (!std::isdigit(input[0])) return std::make_pair(1, std::nullopt);
    std::size_t index{1};
    for (; index < input.size(); ++index) {
        if (!std::isdigit(input[index])) break;
    }
    return std::make_pair(index, std::stoll(input.substr(0, index).data()));
}

auto parse_line(std::string_view line) noexcept -> Comand
{
    auto [parsed, result] = check_number(line);
    assert(result.has_value());
    line = line.substr(parsed + 1);

    Comand operation{.result = result.value()};

    while (1) {
        line = line.substr(1);
        auto [parsed, number] = check_number(line);
        if (!number.has_value()) break;
        operation.operands.push_back(number.value());
        line = line.substr(parsed);
        if (line.empty()) break;
    }

    return operation;
}

void compute_combination(
    const std::vector<Operation>& operation,
    std::int64_t level,
    std::vector<std::vector<Operation>>& operations) noexcept
{
    if (level <= 0) {
        operations.push_back(operation);
        return;
    }
    for (auto op : kOperations) {
        auto new_operation = operation;
        new_operation.push_back(op);
        compute_combination(new_operation, level - 1, operations);
    }
}

auto compute_result(
    std::int64_t lhs,
    std::span<const std::int64_t> remain_operands,
    std::span<const Operation> remain_operations) noexcept -> std::int64_t
{
    assert(remain_operands.size() == remain_operations.size());
    const auto& operation = remain_operations[0];
    auto value = operation(lhs, remain_operands[0]);
    if (remain_operands.size() == 1) return value;
    return compute_result(value, remain_operands.subspan(1), remain_operations.subspan(1));
}

auto check_command(const Comand& command) noexcept
{
    std::vector<std::vector<Operation>> operations_list;
    compute_combination({}, command.operands.size() - 1, operations_list);
    for (const auto& operations : operations_list) {
        auto result = compute_result(command.operands[0], std::span{command.operands}.subspan(1), operations);
        if (result == command.result) return true;
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

    std::vector<Comand> operations;
    parse_file(argv[1], [&](const std::string& line) { operations.push_back(parse_line(line)); });

    auto result = std::ranges::fold_left(operations, 0, [](const auto& sum, const auto& elem) {
        auto valid = check_command(elem);
        return sum + (valid ? elem.result : 0);
    });
    std::cout << result << std::endl;
    return 0;
}

// 3634836
