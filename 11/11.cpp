#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <ranges>
#include <set>
#include <vector>

#include "../common/common.hpp"

constexpr auto kStar = '#';
constexpr std::int64_t kSpaceExtension{1000000 - 1};

using Star = Point;

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

    std::vector<std::int64_t> empty_columns;
    for (std::int64_t i = 0; i < map.front().size(); ++i) {
        if (!std::ranges::all_of(map, [&](const auto& line) { return line[i] != kStar; })) continue;
        empty_columns.push_back(i);
    }

    std::vector<Star> stars;
    std::int64_t empty_line_counter{0};
    for (std::int64_t y = 0; y < map.size(); ++y) {
        if (std::ranges::find(map[y], kStar) == map[y].end()) {
            ++empty_line_counter;
            continue;
        }

        for (std::int64_t x = 0; x < map.front().size(); ++x) {
            if (map[y][x] != kStar) continue;
            auto empty_cols = std::ranges::count_if(empty_columns, [x](auto col) { return col < x; });
            stars.push_back(Star{.x = x + empty_cols * kSpaceExtension, .y = y + empty_line_counter * kSpaceExtension});
        }
    }

    std::vector<std::array<Star, 2>> star_pairs;
    {
        auto star = stars.begin();
        while (star != stars.end()) {
            std::transform(star + 1, stars.end(), std::back_inserter(star_pairs), [&](const auto& star_pair) {
                return std::array{*star, star_pair};
            });
            ++star;
        }
    }

    auto l1_norm = std::ranges::views::transform(
        star_pairs, [](const auto& pair) { return std::abs(pair[1].y - pair[0].y) + std::abs(pair[1].x - pair[0].x); });
    auto result = std::accumulate(l1_norm.begin(), l1_norm.end(), 0);
    std::cout << result << std::endl;
    return 0;
}

// 1323577784 too low
// 1322927930
// 1322927930
// 1322927930

// 1323577784
