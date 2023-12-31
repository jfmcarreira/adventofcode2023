
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <ranges>
#include <set>
#include <vector>

#include "../common/common.hpp"

struct Range
{
    Range(std::int64_t start, std::int64_t count) : start{start}, end{start + count} {}
    std::int64_t start;
    std::int64_t end;
};

struct Mapping
{
    Range source;
    Range destination;
};

using Mappings = std::vector<Mapping>;

auto parse_mapping(std::ifstream& file) noexcept -> Mappings
{
    Mappings mappings;
    std::string line;
    bool skip{true};
    while (std::getline(file, line)) {
        if (skip && !std::isdigit(line[0])) continue;
        if (!skip && !std::isdigit(line[0])) return mappings;
        skip = false;
        auto numbers = parse_line_numbers(line);
        mappings.push_back({.source{numbers[1], numbers[2]}, .destination{numbers[0], numbers[2]}});
    }
    return mappings;
}

auto find_destination(const std::vector<Mapping>& mappings, std::int64_t source) noexcept
{
    auto found = std::ranges::find_if(
        mappings, [&](const auto& map) { return source >= map.source.start && source <= map.source.end; });
    if (found == mappings.end()) return source;
    return found->destination.start + source - found->source.start;
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
    auto card_sep = line.find_first_of(':');
    assert(card_sep != std::string::npos);
    auto seeds_numbers = parse_line_numbers(line.substr(card_sep));

    auto pair_count = seeds_numbers.size() / 2;
    std::vector<Range> seeds;
    for (std::int64_t i = 0; i < pair_count; ++i) {
        seeds.emplace_back(seeds_numbers[i * 2], seeds_numbers[i * 2 + 1]);
    }

    std::vector<Mappings> mappings_steps;
    mappings_steps.push_back(parse_mapping(input_file));
    mappings_steps.push_back(parse_mapping(input_file));
    mappings_steps.push_back(parse_mapping(input_file));
    mappings_steps.push_back(parse_mapping(input_file));
    mappings_steps.push_back(parse_mapping(input_file));
    mappings_steps.push_back(parse_mapping(input_file));
    mappings_steps.push_back(parse_mapping(input_file));

    std::int64_t min_location = std::numeric_limits<std::int64_t>::max();
    for (auto seed_range : seeds) {
        for (auto seed = seed_range.start; seed <= seed_range.end; ++seed) {
            auto source = seed;
            for (auto& step_map : mappings_steps) {
                source = find_destination(step_map, source);
            }
            if (source < min_location) {
                min_location = source;
            }
        }
    }
    std::cout << min_location << std::endl;
    return 0;
}
