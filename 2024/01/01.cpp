#include <algorithm>
#include <fstream>
#include <iostream>
#include <span>
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

auto find_number_repetitions(const std::span<const std::int64_t> range, std::int64_t number) noexcept -> std::int64_t
{
    auto first = std::ranges::find(range, number);
    if (first == range.end()) return 0;
    auto last = std::ranges::find_last(range, number).begin();
    auto count = std::distance(first, last) + 1;
    return count;
};

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
    std::ranges::sort(right_numbers);
    auto result = std::ranges::fold_left(left_numbers, 0, [&](const auto& sum, const auto& elem) {
        auto count = find_number_repetitions(right_numbers, elem);
        return sum + count * elem;
    });
    std::cout << result << std::endl;
    return 0;
}
