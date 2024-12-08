#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

template<typename F>
auto parse_file(const std::filesystem::path& file_name, F&& parsing) noexcept
{
    std::ifstream input_file(file_name, std::ios_base::in);
    std::string line;
    while (std::getline(input_file, line)) {
        parsing(line);
    }
}

inline auto check_numbers(std::string_view line) noexcept -> std::pair<std::int64_t, std::optional<std::int64_t>>
{
    if (line.empty()) return std::make_pair(0, std::nullopt);
    if (!std::isdigit(line[0])) return std::make_pair(1, std::nullopt);
    std::size_t index{1};
    for (; index < line.size(); ++index) {
        if (!std::isdigit(line[index])) break;
    }
    return std::make_pair(index, std::stoll(line.substr(0, index).data()));
}

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

inline auto operator+(const Coord& coord, const Direction& dir) noexcept
{
    return Coord{.y = coord.y + dir.y, .x = coord.x + dir.x};
}

inline auto operator*(const Direction& dir, std::int64_t scale) noexcept
{
    return Direction{
        .x = dir.x * scale,
        .y = dir.y * scale,
    };
}

#endif
