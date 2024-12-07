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

constexpr auto kGuardSymbol = '^';
constexpr auto kObstructionsSymbol = '#';
constexpr auto kCheckedSymbol = 'X';

using Input = std::vector<std::string>;

struct Coord
{
    std::int64_t y{0};
    std::int64_t x{0};

    auto operator==(const Coord&) const noexcept -> bool = default;
};

struct Direction
{
    std::int64_t x{0};
    std::int64_t y{0};

    auto operator==(const Direction&) const noexcept -> bool = default;
};

auto operator+(const Coord& coord, const Direction& dir) noexcept
{
    return Coord{.y = coord.y + dir.y, .x = coord.x + dir.x};
}

auto operator*(const Direction& dir, std::int64_t scale) noexcept
{
    return Direction{
        .x = dir.x * scale,
        .y = dir.y * scale,
    };
}

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
};

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

    auto guard = [&]() -> Guard {
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

    while (is_inside(input, guard.pos)) {
        auto new_pos = guard.pos + guard.dir;
        if (is_obstruction(input, new_pos)) {
            guard.dir = rotate(guard.dir);
        } else {
            guard.pos = new_pos;
            input[new_pos.y][new_pos.x] = kCheckedSymbol;
        }
    }

    auto result = std::ranges::fold_left(std::views::join(input), 0, [](const auto& sum, const auto& character) {
        return sum + (character == kCheckedSymbol ? 1 : 0);
    });
    std::cout << result << std::endl;
    return 0;
}
