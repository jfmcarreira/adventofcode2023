#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
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

constexpr std::string_view kLookupWord{"MAS"};

using Input = std::vector<std::string>;

struct Coord
{
    std::int64_t y{0};
    std::int64_t x{0};

    auto operator==(const Coord&) const noexcept -> bool = default;
};

auto operator<(const Coord& lhs, const Coord& rhs) noexcept
{
    if (lhs.y == rhs.y) return lhs.x < rhs.x;
    return lhs.y < rhs.y;
}

struct Direction
{
    std::int64_t x{0};
    std::int64_t y{0};

    auto operator==(const Direction&) const noexcept -> bool = default;
};

constexpr std::array kDirections{
    Direction{0, -1},
    Direction{0, 1},
    Direction{1, -1},
    Direction{1, 1},
    Direction{-1, 1},
    Direction{-1, -1},
    Direction{1, 0},
    Direction{-1, 0},
};

auto x_cross(const Direction& lhs, const Direction& rhs) noexcept
{
    return (lhs.x == rhs.x && lhs.y == -rhs.y) || (lhs.x == -rhs.x && lhs.y == rhs.y);
}

auto is_diagonal(const Direction& dir) noexcept
{
    return !(dir.x == 0 || dir.y == 0);
}

struct Word
{
    Coord position;
    Direction dir;
    Coord center;
};

auto operator<(const Word& lhs, const Word& rhs) noexcept
{
    if (lhs.center != rhs.center) return lhs.center < rhs.center;
    auto lhs_diagonal = is_diagonal(lhs.dir);
    auto rhs_diagonal = is_diagonal(rhs.dir);
    if (lhs_diagonal != rhs_diagonal) return lhs_diagonal;
    return false;
}

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

auto is_inside(const Input& input, const Coord& pos) noexcept
{
    return pos.x >= 0 && pos.x < input.front().size() && pos.y >= 0 && pos.y < input.size();
}

auto lookup(const Input& input, const Coord& start, const Direction& dir) noexcept -> bool
{
    if (!is_inside(input, start + dir * (std::ssize(kLookupWord) - 1))) {
        return false;
    }

    auto check_char = [&](const Coord& pos, char lookup) noexcept { return input[pos.y][pos.x] == lookup; };
    for (std::int64_t i = 0; i < kLookupWord.size(); ++i) {
        if (!check_char(start + dir * i, kLookupWord[i])) {
            return false;
        }
    }
    return true;
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

    auto line_count = std::ssize(input);
    auto line_width = std::ssize(input.front());

    std::vector<Word> found_words;

    for (std::int64_t y = 0; y < line_count; ++y) {
        for (std::int64_t x = 0; x < line_width; ++x) {
            Coord start{y, x};
            for (const auto& dir : kDirections) {
                if (!lookup(input, start, dir)) continue;
                found_words.emplace_back(start, dir, start + dir);
            }
        }
    }
    std::ranges::sort(found_words, std::less<>{});
    std::vector<Coord> interceptions;
    for (std::int64_t i = 0; i < std::ssize(found_words) - 1; ++i) {
        const auto& current = found_words[i];
        const auto& next = found_words[i + 1];
        if (current.center != next.center) continue;
        if (x_cross(current.dir, next.dir)) {
            interceptions.push_back(current.center);
            continue;
        }
    }
    std::cout << interceptions.size() << std::endl;
    return 0;
}
