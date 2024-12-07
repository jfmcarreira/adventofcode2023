#include <__algorithm/fold.h>

#include <algorithm>
#include <cassert>
#include <fstream>
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

enum class Parsing
{
    Order,
    Rules,
};

struct Ordering
{
    std::set<std::int64_t> before;
    std::set<std::int64_t> after;
};

using Rules = std::unordered_map<std::int64_t, Ordering>;

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

auto parse_order(std::string_view input) noexcept
{
    auto [parsed, lhs] = check_numbers(input);
    auto [_, rhs] = check_numbers(input.substr(parsed + 1));
    assert(lhs.has_value() && rhs.has_value());
    return std::make_pair(lhs.value(), rhs.value());
}

auto parse_rule(std::string_view input) noexcept
{
    std::vector<std::int64_t> sequence;
    while (1) {
        auto [parsed, number] = check_numbers(input);
        if (!number.has_value()) return sequence;
        sequence.push_back(number.value());
        if (input.size() == parsed) return sequence;
        input = input.substr(parsed + 1);
    }
    return sequence;
}

auto lookup(const std::set<std::int64_t>& set)
{
    return [&](const auto& page) { return std::ranges::find(set, page) == set.end(); };
}

auto validate(const Rules& rules, std::span<const std::int64_t> sequence) noexcept
{
    for (std::int64_t page_index = 0; page_index < sequence.size(); ++page_index) {
        auto page_number = sequence[page_index];

        auto found_page = rules.find(page_number);
        assert(found_page != rules.end());
        const auto& rule = found_page->second;

        if (!std::ranges::all_of(sequence.subspan(0, page_index), lookup(rule.before))) {
            return false;
        }
        if (!std::ranges::all_of(sequence.subspan(page_index + 1), lookup(rule.after))) {
            return false;
        }
    }
    return true;
}

auto order_sequence(const Rules& rules, std::vector<std::int64_t> sequence) noexcept
{
    auto process = [&]() {
        for (std::int64_t page_index = 0; page_index < sequence.size(); ++page_index) {
            auto page_number = sequence[page_index];

            auto found_page = rules.find(page_number);
            assert(found_page != rules.end());
            const auto& rule = found_page->second;

            for (std::int64_t i = 0; i < page_index; ++i) {
                if (std::ranges::find(rule.before, sequence[i]) != rule.before.end()) {
                    std::swap(sequence[page_index], sequence[i]);
                    return false;
                }
            }

            for (std::int64_t i = page_index + 1; i < sequence.size(); ++i) {
                if (std::ranges::find(rule.after, sequence[i]) != rule.after.end()) {
                    std::swap(sequence[page_index], sequence[i]);
                    return false;
                }
            }
        }
        return true;
    };

    while (1) {
        if (process()) break;
    }
    return sequence;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Missing input!" << std::endl;
        return 1;
    }
    std::ifstream input_file(argv[1], std::ios_base::in);

    Rules order_rules;
    std::vector<std::vector<std::int64_t>> page_sequences;

    auto parsing = Parsing::Order;
    parse_file(argv[1], [&](const std::string& line) {
        if (line.empty()) {
            parsing = Parsing::Rules;
            return;
        }
        switch (parsing) {
            case Parsing::Order: {
                auto [before, after] = parse_order(line);
                order_rules[before].before.insert(after);
                order_rules[after].after.insert(before);
            } break;
            case Parsing::Rules:
                page_sequences.push_back(parse_rule(line));
                break;
        }
    });

    for (auto& sequence : page_sequences) {
        order_sequence(order_rules, sequence);
    }

    auto result = std::ranges::fold_left(page_sequences, 0, [&](const std::int64_t& sum, const auto& sequence) {
        if (validate(order_rules, sequence)) return sum;
        auto new_sequence = order_sequence(order_rules, sequence);
        return sum + new_sequence[new_sequence.size() / 2];
    });
    std::cout << result << std::endl;
    return 0;
}
