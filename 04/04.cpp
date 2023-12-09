
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <vector>

struct Card
{
    std::int64_t id;
    std::vector<std::int64_t> game_numbers;
    std::vector<std::int64_t> winning_numbers;
};

auto check_numbers(std::string_view line) noexcept -> std::pair<std::int64_t, std::optional<std::int64_t>>
{
    if (!std::isdigit(line[0])) return std::make_pair(1, std::nullopt);
    std::size_t index{1};
    for (; index < line.size(); ++index) {
        if (!std::isdigit(line[index])) break;
    }
    return std::make_pair(index, std::stoll(line.substr(0, index).data()));
}

auto parse_numbers(std::string_view current_line) noexcept
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

auto parse_card(std::string_view line) noexcept
{
    auto card_sep = line.find_first_of(':');
    assert(card_sep != std::string::npos);

    auto id_sep = line.substr(0, card_sep).find_first_of(' ');
    assert(id_sep != std::string::npos);

    auto numbers = line.substr(card_sep);
    auto numbers_sep = numbers.find_first_of('|');
    assert(numbers_sep != std::string::npos);
    return Card{
        .id = std::stoll(line.substr(0, card_sep).substr(id_sep).data()),
        .game_numbers = parse_numbers(numbers.substr(0, numbers_sep)),
        .winning_numbers = parse_numbers(numbers.substr(numbers_sep))};
}

auto card_points(const Card& card) -> std::int64_t
{
    std::int64_t matches{0};
    for (auto game_num : card.game_numbers) {
        if (std::ranges::find(card.winning_numbers, game_num) != card.winning_numbers.end()) {
            ++matches;
        }
    }
    if (matches == 0) return 0;
    return 1 << (matches - 1);
}

auto card_matches(const Card& card) -> std::int64_t
{
    std::int64_t matches{0};
    for (auto game_num : card.game_numbers) {
        if (std::ranges::find(card.winning_numbers, game_num) != card.winning_numbers.end()) {
            ++matches;
        }
    }
    return matches;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Missing input!" << std::endl;
        return 1;
    }

    std::ifstream input_file(argv[1], std::ios_base::in);

    std::vector<Card> cards;

    std::string line;
    while (std::getline(input_file, line)) {
        cards.push_back(parse_card(line));
    }

    // for (auto& card : cards) {
    //     std::cout << card.id;
    //     std::cout << "|";
    //     for (auto n : card.game_numbers) {
    //         std::cout << n << ",";
    //     }
    //     std::cout << "|";
    //     for (auto n : card.winning_numbers) {
    //         std::cout << n << ",";
    //     }
    //     std::cout << std::endl;
    // }

    std::vector<std::int64_t> cards_points;
    std::ranges::transform(cards, std::back_inserter(cards_points), card_points);

    auto result_part_one = std::accumulate(cards_points.begin(), cards_points.end(), 0);
    std::cout << result_part_one << std::endl;

    std::int64_t result_part_two{0};
    auto duplicated_cards = cards;
    duplicated_cards.reserve(duplicated_cards.size() + cards.size() * cards.front().game_numbers.size());

    for (std::int64_t c = 0; c < duplicated_cards.size(); ++c) {
        ++result_part_two;
        const auto card = duplicated_cards[c];
        auto matches = card_matches(card);
        for (std::int64_t i = 0; i < matches; ++i) {
            assert(card.id + i < cards.size());
            duplicated_cards.push_back(cards[card.id + i]);
        }
    }
    std::cout << result_part_two << std::endl;
    return 0;
}

// 21568
