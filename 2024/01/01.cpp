#include <algorithm>
#include <cassert>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <ostream>
#include <ranges>
#include <string>
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

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Missing input!" << std::endl;
        return 1;
    }
    std::ifstream input_file(argv[1], std::ios_base::in);

    std::vector<std::int64_t> left_numbers;
    std::vector<std::int64_t> right_numbers;

    auto parsing = [&](const std::string& line) {
        std::size_t pos = line.find(" ");
        left_numbers.push_back(std::stoll(line.substr(0, pos)));
        right_numbers.push_back(std::stoll(line.substr(pos + 1)));
    };

    parse_file(argv[1], parsing);

    std::ranges::sort(left_numbers);
    std::ranges::sort(right_numbers);

    auto line_count = std::min(left_numbers.size(), right_numbers.size());

    std::int64_t result{0};
    for (std::int64_t i = 0; i < line_count; ++i) {
        result += std::abs(right_numbers[i] - left_numbers[i]);
    }

    std::cout << result << std::endl;

    return 0;
}
