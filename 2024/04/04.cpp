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

constexpr std::string_view kLookupWord{"XMAS"};

using Input = std::vector<std::string>;

struct Coord
{
    std::int64_t x{0};
    std::int64_t y{0};

    auto operator==(const Coord&) const noexcept -> bool = default;
};

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

struct Word
{
    Coord position;
    Direction dir;
};

auto operator+(const Coord& coord, const Direction& dir) noexcept
{
    return Coord{.x = coord.x + dir.x, .y = coord.y + dir.y};
}

auto operator*(const Direction& dir, std::int64_t scale) noexcept
{
    return Direction{.x = dir.x * scale, .y = dir.y * scale};
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
            Coord start{x, y};
            for (const auto& dir : kDirections) {
                if (!lookup(input, start, dir)) continue;
                found_words.emplace_back(start, dir);
            }
        }
    }
    std::cout << found_words.size() << std::endl;
    return 0;
}
