

#include <optional>
#include <string>
#include <string_view>
#include <utility>

inline auto check_numbers(std::string_view line) noexcept -> std::pair<std::int64_t, std::optional<std::int64_t>>
{
    if (!std::isdigit(line[0])) return std::make_pair(1, std::nullopt);
    std::size_t index{1};
    for (; index < line.size(); ++index) {
        if (!std::isdigit(line[index])) break;
    }
    return std::make_pair(index, std::stoll(line.substr(0, index).data()));
}

inline auto parse_line_numbers(std::string_view current_line) noexcept
{
    std::vector<std::int64_t> numbers;
    std::int64_t index{0};
    while (index < current_line.size()) {
        auto [parsed, number] = check_numbers(current_line.substr(index));
        if (!number.has_value()) {
            ++index;
            continue;
        }
        numbers.push_back(number.value());
        index += parsed;
    }
    return numbers;
}
