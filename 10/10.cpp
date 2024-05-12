#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <numeric>
#include <optional>
#include <ostream>
#include <ranges>
#include <set>
#include <type_traits>
#include <vector>

#include "../common/common.hpp"

enum class Direction
{
    Start,
    North,
    South,
    West,
    East,
};

using MapElement = std::optional<std::array<Direction, 2>>;
using Map = std::vector<std::vector<MapElement>>;

using TileElement = int64_t;
using Tiles = std::vector<std::vector<TileElement>>;

struct PathDirection
{
    Point pos;
    Direction target_dir;
    Direction source_dir;
};

template<typename T>
auto retrieve_pos(std::vector<std::vector<T>>& map, const Point& pos) noexcept -> T&
{
    return map[pos.y][pos.x];
}

template<typename T>
auto retrieve_pos(const std::vector<std::vector<T>>& map, const Point& pos) noexcept -> const T&
{
    return map[pos.y][pos.x];
}

constexpr std::array kStartPosition = {Direction::Start, Direction::Start};

auto process_line(std::string_view line) noexcept -> std::vector<MapElement>
{
    std::vector<MapElement> map_line;
    map_line.reserve(line.size());
    auto convert_chars = [](char c) -> std::optional<std::array<Direction, 2>> {
        switch (c) {
            case '-':
                return std::array{Direction::West, Direction::East};
            case '|':
                return std::array{Direction::North, Direction::South};
            case 'L':
                return std::array{Direction::North, Direction::East};
            case 'J':
                return std::array{Direction::North, Direction::West};
            case '7':
                return std::array{Direction::South, Direction::West};
            case 'F':
                return std::array{Direction::South, Direction::East};
            case 'S':
                return std::array{Direction::Start, Direction::Start};
        };
        return {};
    };
    std::ranges::transform(line, std::back_inserter(map_line), convert_chars);
    return map_line;
}

auto find_start_point(const Map& map) noexcept -> Point
{
    auto map_line = map.begin();
    while (map_line != map.end()) {
        auto map_column = std::ranges::find(*map_line, kStartPosition);
        if (map_column == map_line->end()) {
            ++map_line;
            continue;
        }
        return {.x = std::distance(map_line->begin(), map_column), .y = std::distance(map.begin(), map_line)};
    }
    return {};
}

auto next_coordinate(const PathDirection& path) noexcept
{
    Point next_point = path.pos;
    switch (path.target_dir) {
        using enum Direction;
        case North:
            next_point.y--;
            break;
        case South:
            next_point.y++;
            break;
        case West:
            next_point.x--;
            break;
        case East:
            next_point.x++;
            break;
        default:
            assert(false);
    }
    return next_point;
}

auto matching_direction(Direction dir) noexcept
{
    switch (dir) {
        using enum Direction;
        case North:
            return South;
        case South:
            return North;
        case West:
            return East;
        case East:
            return West;
        default:
            assert(false);
    }
}

auto rotate_direction(Direction dir, bool clockwise) noexcept
{
    switch (dir) {
        using enum Direction;
        case North:
            return clockwise ? East : West;
        case South:
            return clockwise ? West : East;
        case West:
            return clockwise ? North : South;
        case East:
            return clockwise ? South : North;
        default:
            assert(false);
    }
}

auto is_clockwise(const PathDirection& path_direction) noexcept
{
    auto target = path_direction.target_dir;
    if (path_direction.source_dir == target) return 0;

    switch (path_direction.source_dir) {
        using enum Direction;
        case North:
            return target == East ? 1 : -1;
        case South:
            return target == West ? 1 : -1;
        case West:
            return target == North ? 1 : -1;
        case East:
            return target == South ? 1 : -1;
        default:
            assert(false);
    }
}

auto next_path(const Map& map, const PathDirection& current) noexcept -> std::optional<PathDirection>
{
    auto next_pos = next_coordinate(current);
    if (next_pos.y < 0 || next_pos.y >= map.size()) return std::nullopt;
    if (next_pos.x < 0 || next_pos.x >= map.front().size()) return std::nullopt;

    const auto& map_pos = retrieve_pos(map, next_pos);
    if (!map_pos.has_value()) return std::nullopt;

    if (map_pos == kStartPosition) {
        return PathDirection{
            .pos = next_pos,
            .target_dir = Direction::Start,
            .source_dir = current.target_dir,
        };
    }

    auto found_dir = std::ranges::find(map_pos.value(), matching_direction(current.target_dir));
    if (found_dir == map_pos->end()) return std::nullopt;

    auto new_dir = [&]() {
        if (found_dir != map_pos->begin()) return map_pos->front();
        return *(map_pos->begin() + 1);
    }();

    return PathDirection{
        .pos = next_pos,
        .target_dir = new_dir,
        .source_dir = current.target_dir,
    };
}

auto next_path_any_direction(const Map& map, const Point& current) noexcept
{
    auto north_pos = next_path(map, {current, Direction::North});
    if (north_pos.has_value()) return north_pos.value();

    auto west_pos = next_path(map, {current, Direction::West});
    if (west_pos.has_value()) return west_pos.value();

    auto east_pos = next_path(map, {current, Direction::East});
    if (east_pos.has_value()) return east_pos.value();

    auto south_pos = next_path(map, {current, Direction::East});
    assert(south_pos.has_value());
    return south_pos.value();
}

void paint_tiles(
    const Map& map,
    const std::vector<PathDirection>& path,
    const PathDirection& current,
    Tiles& tiles) noexcept
{
    auto next_pos = next_coordinate(current);
    while (1) {
        if (next_pos.y < 0 || next_pos.y >= map.size()) return;
        if (next_pos.x < 0 || next_pos.x >= map.front().size()) return;

        auto found_path = std::ranges::find(path, next_pos, &PathDirection::pos);
        if (found_path != path.end()) return;

        auto& tile = retrieve_pos(tiles, next_pos);
        tile = 1;

        next_pos = next_coordinate({next_pos, current.target_dir});
    }
}

void paint_tiles(
    const Map& map,
    const std::vector<PathDirection>& path,
    const PathDirection& current,
    bool clockwise,
    Tiles& tiles) noexcept
{
    auto source_dir = current.source_dir;
    paint_tiles(map, path, {current.pos, rotate_direction(source_dir, clockwise)}, tiles);

    auto target_dir = current.target_dir;
    if (target_dir == source_dir || target_dir == Direction::Start) return;
    paint_tiles(map, path, {current.pos, rotate_direction(target_dir, clockwise)}, tiles);
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Missing input!" << std::endl;
        return 1;
    }

    Map map;
    std::ifstream input_file(argv[1], std::ios_base::in);
    std::string line;
    while (std::getline(input_file, line)) {
        map.push_back(process_line(line));
    }

    auto initial_direction = next_path_any_direction(map, find_start_point(map));
    auto path_direction = initial_direction;

    std::vector<PathDirection> path{initial_direction};
    while (1) {
        auto new_path = next_path(map, path_direction);
        if (!new_path.has_value()) {
            path_direction = initial_direction;
            path = {initial_direction};
            continue;
        }
        path_direction = new_path.value();
        path.push_back(path_direction);

        if (retrieve_pos(map, path_direction.pos) == kStartPosition) {
            break;
        }
    }
    std::cout << path.size() / 2 << std::endl;

    path.back().target_dir = path.front().source_dir;

    std::int64_t clockwise_turns{0};
    for (auto& p : path) {
        clockwise_turns += is_clockwise(p);
    }
    assert(clockwise_turns != 0);

    auto rotate_clockwise = clockwise_turns > 0;

    Tiles tiles;
    tiles.resize(map.size());
    for (auto& t : tiles) {
        t.resize(map.front().size(), 0);
    }

    for (auto& p : path | std::views::reverse) {
        paint_tiles(map, path, p, rotate_clockwise, tiles);
    }

    std::int64_t result_part_two{0};
    for (const auto& line : tiles) {
        result_part_two += std::accumulate(line.begin(), line.end(), 0, std::plus<>());
        for (const auto& t : line) {
            std::cout << t;
        }
        std::cout << std::endl;
    }
    std::cout << result_part_two << std::endl;

    return 0;
}
