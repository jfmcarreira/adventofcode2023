
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <ostream>
#include <set>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "../common.hpp"

constexpr auto kGuardSymbol = '^';
constexpr auto kNormalSymbol = '.';
constexpr auto kObstructionsSymbol = '#';
constexpr auto kCheckedSymbol = 'X';

using Input = std::vector<std::string>;

auto rotate(const Direction& dir) noexcept
{
    if (dir.x == 1) {
        return Direction{.x = 0, .y = 1};
    } else if (dir.x == -1) {
        return Direction{.x = 0, .y = -1};
    } else if (dir.y == 1) {
        return Direction{.x = -1, .y = 0};
    }
    return Direction{.x = 1, .y = 0};
}

auto is_inside(const Input& input, const Coord& pos) noexcept
{
    return pos.x >= 0 && pos.x < input.front().size() && pos.y >= 0 && pos.y < input.size();
}

auto is_obstruction(const Input& input, const Coord& pos) noexcept
{
    if (!is_inside(input, pos)) {
        return false;
    }
    return input[pos.y][pos.x] == kObstructionsSymbol;
}

struct Guard
{
    Coord pos;
    Direction dir;

    auto operator==(const Guard&) const noexcept -> bool = default;
};

auto is_loop(const Input& input, Guard guard)
{
    std::vector<Guard> sequence;
    while (is_inside(input, guard.pos)) {
        sequence.push_back(guard);
        auto new_pos = guard.pos + guard.dir;
        if (is_obstruction(input, new_pos)) {
            guard.dir = rotate(guard.dir);
        } else {
            guard.pos = new_pos;
        }
        if (std::ranges::find(sequence, guard) != sequence.end()) return true;
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

    Input input;
    parse_file(argv[1], [&](const std::string& line) { input.push_back(line); });

    auto map_height = std::ssize(input);
    auto map_width = std::ssize(input.front());

    const auto guard = [&]() -> Guard {
        for (std::int64_t y = 0; y < map_height; ++y) {
            auto found_guard = input[y].find(kGuardSymbol);
            if (found_guard == std::string::npos) continue;
            return {
                .pos =
                    Coord{
                        .y = y,
                        .x = static_cast<std::int64_t>(found_guard),
                    },
                .dir = Direction{
                    .x = 0,
                    .y = -1,
                }};
        }
        return {};
    }();

    {
        auto checking_guard = guard;
        while (is_inside(input, checking_guard.pos)) {
            auto new_pos = checking_guard.pos + checking_guard.dir;
            if (is_obstruction(input, new_pos)) {
                checking_guard.dir = rotate(checking_guard.dir);
            } else {
                checking_guard.pos = new_pos;
                input[new_pos.y][new_pos.x] = kCheckedSymbol;
            }
        }
    }

    std::int64_t result{0};
    for (std::int64_t y = 0; y < map_height; ++y) {
        for (std::int64_t x = 0; x < map_width; ++x) {
            if (input[y][x] != kCheckedSymbol) continue;
            if (is_obstruction(input, Coord{.y = y, .x = x})) continue;
            input[y][x] = kObstructionsSymbol;
            if (is_loop(input, guard)) {
                ++result;
            }
            input[y][x] = kNormalSymbol;
        }
    }

    std::cout << result << std::endl;
    return 0;
}
