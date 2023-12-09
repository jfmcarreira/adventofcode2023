
#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <ostream>
#include <ranges>
#include <set>
#include <vector>

#include "../common/common.hpp"

constexpr std::string_view kDestination{"ZZZ"};

struct Coordinate
{
    std::string start;
    std::array<std::string, 2> destination;
};

auto parse_line(std::string_view line) noexcept
{
    Coordinate coordinate;
    auto sep = line.find_first_of('=');
    assert(sep != std::string::npos);
    coordinate.start = line.substr(0, 3);
    coordinate.destination[0] = line.substr(7, 3);
    coordinate.destination[1] = line.substr(12, 3);
    return coordinate;
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

    std::vector<Coordinate> map;

    std::string line;
    std::getline(input_file, line);
    while (std::getline(input_file, line)) {
        map.push_back(parse_line(line));
    }

    // for (auto i : directions) {
    //     std::cout << i << ",";
    // }
    // std::cout << std::endl;
    // for (auto& m : map) {
    //     std::cout << m.start;
    //     std::cout << "|";
    //     std::cout << m.destination[0];
    //     std::cout << "|";
    //     std::cout << m.destination[1];
    //     std::cout << std::endl;
    // }

    std::int64_t run{0};

    auto position = map.begin();
    while (1) {
        auto inst = directions[run % directions.size()];
        ++run;
        std::string_view dest{position->destination[inst]};
        // std::cout << dest << std::endl;
        if (dest == kDestination) break;
        position = std::ranges::find(map, dest, &Coordinate::start);
        assert(position != map.end());
    }
    std::cout << run << std::endl;
    return 0;
}
