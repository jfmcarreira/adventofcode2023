
#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <vector>

class Parser
{
public:
    Parser() noexcept { lines_cache_.resize(3, ""); }

    auto checkNumbers(std::string_view line) noexcept -> std::pair<std::int64_t, std::optional<std::int64_t>>
    {
        auto is_num = std::isdigit(line[0]);
        if (!is_num) return std::make_pair(1, std::nullopt);

        std::size_t index{1};
        for (; index < line.size(); ++index) {
            auto c = line[index];
            if (!std::isdigit(c)) break;
        }

        auto parsed_number = line.substr(0, index);
        auto number = std::stoll(parsed_number.data());
        return std::make_pair(index, number);
    }

    auto checkSymbolAround(std::int64_t start, std::int64_t width) noexcept -> bool
    {
        auto check_start = std::max(start - 1, 0LL);

        auto is_symbol = [](char c) { return c != '.' && !std::isdigit(c); };

        auto check_symbols = [&](auto& line) {
            if (line.empty()) return false;
            auto check_end = std::min(start + width, static_cast<std::int64_t>(line.size()) - 1);
            return std::any_of(line.begin() + check_start, line.begin() + check_end + 1, is_symbol);
        };

        for (auto& line : lines_cache_) {
            if (check_symbols(line)) return true;
        }

        return false;
    }

    auto parseLine(std::string_view line) noexcept -> std::vector<std::int64_t>
    {
        lines_cache_[2] = line;
        std::string_view current_line{lines_cache_[1]};
        if (current_line.empty()) {
            std::rotate(lines_cache_.begin(), lines_cache_.begin() + 1, lines_cache_.end());
            return {};
        }

        std::vector<std::int64_t> parts_found;
        parts_found.reserve(8);

        std::int64_t index{0};
        while (index < current_line.size()) {
            auto [parsed, number] = checkNumbers(current_line.substr(index));
            if (number.has_value() && checkSymbolAround(index, parsed)) {
                parts_found.push_back(number.value());
            }
            index += parsed;
        }
        std::rotate(lines_cache_.begin(), lines_cache_.begin() + 1, lines_cache_.end());
        return parts_found;
    }

private:
    std::vector<std::string> lines_cache_;
};

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Missing input!" << std::endl;
        return 1;
    }

    std::ifstream input_file(argv[1], std::ios_base::in);

    Parser engine_parser;
    std::int64_t result{0};

    std::string line;
    while (std::getline(input_file, line)) {
        auto parts_found = engine_parser.parseLine(line);
        result = std::accumulate(parts_found.begin(), parts_found.end(), result);
    }
    auto parts_found = engine_parser.parseLine(std::string{});
    result = std::accumulate(parts_found.begin(), parts_found.end(), result);

    std::cout << result << std::endl;
    return 0;
}

// 586783
// 529319
// 527749
// 528534
// 527992
// 527472
// 512449

// VC 528799
