#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <ranges>
#include <set>
#include <vector>

#include "../common/common.hpp"

constexpr auto kStar = '#';

struct Star
{
    Point pos;
    auto operator==(const Star&) const noexcept -> bool = default;
    auto operator<(const Star& rhs) const noexcept -> bool { return pos < rhs.pos; }
};

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Missing input!" << std::endl;
        return 1;
    }

    std::vector<std::string> map;
    {
        std::ifstream input_file(argv[1], std::ios_base::in);
        std::string line;
        while (std::getline(input_file, line)) {
            map.push_back(line);
        }
    }

    for (std::int64_t i = 0; i < map.front().size(); ++i) {
        auto no_star = std::ranges::all_of(map, [&](const auto& line) { return line[i] != kStar; });
        if (!no_star) continue;
        for (auto& line : map) {
            line.insert(line.begin() + i, '.');
        }
        ++i;
    }

    {
        auto line = map.begin();
        while (line != map.end()) {
            if (std::ranges::find(*line, kStar) == line->end()) {
                line = map.insert(line, *line);
                ++line;
            }
            ++line;
        }
    }

    for (const auto& line : map) {
        std::cout << line << std::endl;
    }

    std::vector<Star> stars;
    for (std::int64_t y = 0; y < map.size(); ++y) {
        for (std::int64_t x = 0; x < map.front().size(); ++x) {
            if (map[y][x] != kStar) continue;
            stars.push_back(Star{.pos = {.x = x, .y = y}});
        }
    }

    std::vector<std::set<Star>> star_pairs;
    {
        auto star = stars.begin();
        while (star != stars.end()) {
            std::transform(star + 1, stars.end(), std::back_inserter(star_pairs), [&star](const auto& star_pair) {
                return std::set{*star, star_pair};
            });
            ++star;
        }
    }

    std::ranges::sort(star_pairs);
    star_pairs.erase(std::unique(star_pairs.begin(), star_pairs.end()), star_pairs.end());

    auto l1_norm = std::ranges::views::transform(star_pairs, [&](auto& pair) {
        return std::abs(pair.begin()->pos.y - pair.rbegin()->pos.y) +
               std::abs(pair.begin()->pos.x - pair.rbegin()->pos.x);
    });
    auto result_part_one = std::accumulate(l1_norm.begin(), l1_norm.end(), 0, std::plus<>());
    std::cout << result_part_one << std::endl;
    return 0;
}
