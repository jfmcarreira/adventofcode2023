#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>
#include <ranges>
#include <set>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "../common.hpp"

constexpr auto kSkipSymbol{'.'};

struct Antenna
{
    Coord pos;
    char sym{' '};

    auto operator==(const Antenna&) const noexcept -> bool = default;
};

using AntennaPair = std::pair<const Antenna*, const Antenna*>;

auto antenna_pairing(const std::vector<Antenna>& antennas) noexcept
{
    std::vector<AntennaPair> antenna_pairs;

    for (auto it = antennas.begin(); it < antennas.end(); ++it) {
        auto sym = it->sym;
        std::span remaining{it + 1, antennas.end()};
        auto filtered_antennas =
            std::views::filter(remaining, [&sym](const auto& antenna) { return antenna.sym == sym; });
        for (const auto& match : filtered_antennas) {
            antenna_pairs.push_back(AntennaPair{&(*it), &match});
        }
    }

    return antenna_pairs;
}

auto operator-(const Coord& lhs, const Coord& rhs) noexcept
{
    return Direction{
        .x = lhs.x - rhs.x,
        .y = lhs.y - rhs.y,
    };
}

auto operator<(const Coord& lhs, const Coord& rhs) noexcept
{
    if (lhs.y == rhs.y) return lhs.x < rhs.x;
    return lhs.y < rhs.y;
}

auto is_inside(const std::vector<std::string>& input, const Coord& pos) noexcept
{
    return pos.x >= 0 && pos.x < input.front().size() && pos.y >= 0 && pos.y < input.size();
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Missing input!" << std::endl;
        return 1;
    }
    std::ifstream input_file(argv[1], std::ios_base::in);

    std::vector<std::string> map;
    parse_file(argv[1], [&](const std::string& line) { map.push_back(line); });

    std::vector<Antenna> antennas;
    for (std::int64_t y = 0; y < map.size(); ++y) {
        const auto& line = map[y];
        for (std::int64_t i = 0; i < line.size(); ++i) {
            if (line[i] == kSkipSymbol) continue;
            antennas.push_back(Antenna{
                .pos = {.y = y, .x = i},
                .sym = line[i],
            });
        }
    }

    auto antenna_pairs = antenna_pairing(antennas);

    std::set<Coord> antinodes;

    for (const auto& [src, dest] : antenna_pairs) {

        auto add_antinode = [&](const auto& start, const auto& dir, const auto& other) {
            auto target = start->pos + dir;
            if (!is_inside(map, target)) return;
            if (target == start->pos) return;
            if (target == other->pos) return;
            antinodes.insert(target);
        };

        auto direction = src->pos - dest->pos;
        add_antinode(src, direction, dest);
        add_antinode(src, direction * -1, dest);
        add_antinode(dest, direction, src);
        add_antinode(dest, direction * -1, src);
    }

    std::cout << antinodes.size() << std::endl;

    return 0;
}
