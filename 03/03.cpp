
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <ranges>
#include <string>
#include <vector>

struct EngineSymbol
{
    std::int64_t x;
    std::int64_t y;
    char symbol;
};

struct EnginePart
{
    std::int64_t number;
    std::vector<EngineSymbol> symbols;
};

auto check_numbers(std::string_view line) noexcept -> std::pair<std::int64_t, std::optional<std::int64_t>>
{
    if (!std::isdigit(line[0])) return std::make_pair(1, std::nullopt);
    std::size_t index{1};
    for (; index < line.size(); ++index) {
        auto c = line[index];
        if (!std::isdigit(c)) break;
    }
    return std::make_pair(index, std::stoll(line.substr(0, index).data()));
}

auto check_part(const EnginePart& ref_part, std::span<EnginePart> remaining_parts) -> std::int64_t
{
    for (const auto& sym : ref_part.symbols) {
        for (const auto& part : remaining_parts) {
            if (std::ranges::any_of(part.symbols, [&](const auto& s) { return s.x == sym.x && s.y == sym.y; })) {
                return part.number * ref_part.number;
            }
        }
    }
    return 0;
};

class GearParser
{
public:
    GearParser() noexcept { lines_cache_.resize(3, ""); }

    auto checkSymbolAround(std::int64_t start, std::int64_t width) noexcept -> std::vector<EngineSymbol>
    {
        // auto is_symbol = [](auto c) { return c != '.' && !std::isdigit(c); };
        auto is_symbol = [](auto c) { return c == '*'; };

        std::vector<EngineSymbol> symbols;
        auto check_start = std::max(start - 1, 0LL);
        for (std::int64_t l = 0; l < lines_cache_.size(); ++l) {
            auto& line = lines_cache_[l];
            if (line.empty()) continue;
            auto check_end = std::min(start + width + 1, static_cast<std::int64_t>(line.size()));
            for (auto i = check_start; i < check_end; ++i) {
                char c = line[i];
                if (!is_symbol(c)) continue;
                symbols.push_back({.x = i, .y = line_number_ + l - 1, .symbol = c});
            }
        }
        return symbols;
    }

    auto parseLine(std::string_view line) noexcept -> std::vector<EnginePart>
    {
        lines_cache_[2] = line;
        std::string_view current_line{lines_cache_[1]};
        if (current_line.empty()) {
            std::rotate(lines_cache_.begin(), lines_cache_.begin() + 1, lines_cache_.end());
            return {};
        }

        std::vector<EnginePart> parts_found;
        parts_found.reserve(8);

        std::int64_t index{0};
        while (index < current_line.size()) {
            auto [parsed, number] = check_numbers(current_line.substr(index));
            if (!number.has_value()) {
                ++index;
                continue;
            }
            auto symbols = checkSymbolAround(index, parsed);
            index += parsed;
            if (symbols.empty()) continue;
            parts_found.push_back({.number = number.value(), .symbols = std::move(symbols)});
        }
        std::rotate(lines_cache_.begin(), lines_cache_.begin() + 1, lines_cache_.end());
        ++line_number_;
        return parts_found;
    }

private:
    std::vector<std::string> lines_cache_;
    std::int64_t line_number_{0};
};

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Missing input!" << std::endl;
        return 1;
    }

    std::ifstream input_file(argv[1], std::ios_base::in);

    GearParser gear_parser;
    std::vector<EnginePart> engine_parts;
    std::string line;
    while (std::getline(input_file, line)) {
        auto parts_found = gear_parser.parseLine(line);
        std::ranges::transform(parts_found, std::back_inserter(engine_parts), [](auto& part) { return part; });
    }
    auto parts_found = gear_parser.parseLine(std::string{});
    std::ranges::transform(parts_found, std::back_inserter(engine_parts), [](auto& part) { return part; });
    auto result_part_one = std::accumulate(engine_parts.begin(), engine_parts.end(), 0, [](auto acc, const auto& part) {
        acc += part.number;
        return acc;
    });
    std::cout << result_part_one << std::endl;

    std::int64_t result_part_two{0};
    auto it = engine_parts.begin();
    while (it != engine_parts.end()) {
        result_part_two += check_part(*it, std::span{engine_parts}.subspan(1));
        engine_parts.erase(it);
    }

    std::cout << result_part_two << std::endl;
    return 0;
}
