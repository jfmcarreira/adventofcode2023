#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <ostream>
#include <ranges>
#include <set>
#include <vector>

#include "../common/common.hpp"

using Coordinate = std::array<std::string, 2>;

struct PathPoint
{
    std::string coordinate;
    std::int64_t offset;
};

struct PathLoop
{
    std::vector<PathPoint> loop;
    std::int64_t offset;
};

auto compute_path_loop(
    const std::vector<std::int64_t>& directions,
    const std::map<std::string, Coordinate>& map,
    const std::string& start_coordinate) noexcept
{
    PathLoop path;
    std::vector<PathPoint> temp_path;

    std::int64_t run{0};
    auto position = map.find(start_coordinate);
    while (1) {
        if (run == 0) {
            auto found_start = std::ranges::find(temp_path, position->first, &PathPoint::coordinate);
            if (found_start != temp_path.end() && found_start->offset == run) {
                path.offset = std::distance(temp_path.begin(), found_start);
                path.loop.resize(std::distance(found_start, temp_path.end()));
                std::copy(found_start, temp_path.end(), path.loop.begin());
                break;
            }
        }

        temp_path.push_back({.coordinate = position->first, .offset = run});

        auto inst = directions[run];
        position = map.find(position->second[inst]);
        run = (run + 1) % directions.size();
        assert(position != map.end());
    }
    return path;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Missing input!" << std::endl;
        return 1;
    }

    std::ifstream input_file(argv[1], std::ios_base::in);

    std::vector<std::int64_t> directions;
    {
        std::string line;
        std::getline(input_file, line);
        directions.reserve(line.size());
        std::ranges::transform(line, std::back_inserter(directions), [](auto c) { return c == 'L' ? 0 : 1; });
    }

    std::map<std::string, Coordinate> map;
    {
        std::string line;
        std::getline(input_file, line);
        while (std::getline(input_file, line)) {
            Coordinate destination;
            destination[0] = line.substr(7, 3);
            destination[1] = line.substr(12, 3);
            map[std::string{line.substr(0, 3)}] = std::move(destination);
        }
    }

    std::vector<PathLoop> paths;
    auto pos = map.begin();
    while (pos != map.end()) {
        if (pos->first.back() == 'A') {
            paths.push_back(compute_path_loop(directions, map, pos->first));
        }
        ++pos;
    }

    std::ranges::sort(paths, [](const auto& lhs, const auto& rhs) { return lhs.loop.size() < rhs.loop.size(); });

    auto is_end = [](const auto& path) { return path.coordinate.back() == 'Z'; };
    auto count = std::ranges::count_if(paths[0].loop, is_end);
    assert(count == 1);
    auto offset = std::distance(paths[0].loop.begin(), std::ranges::find_if(paths[0].loop, is_end));

    std::int64_t repetitions{0};
    while (1) {
        auto ref_offset = paths[0].offset + repetitions * paths[0].loop.size() + offset;
        bool reached{true};
        for (const auto& path : paths | std::ranges::views::drop(1)) {
            std::int64_t test_index = (ref_offset - path.offset) % path.loop.size();
            reached &= path.loop[test_index].coordinate.back() == 'Z';
            if (!reached) break;
        }
        if (reached) {
            std::cout << ref_offset << std::endl;
            return 0;
        }
        ++repetitions;
    }
    return 0;
}

// 13524038372771
