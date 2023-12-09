#include <algorithm>
#include <cassert>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <ostream>
#include <ranges>
#include <set>
#include <vector>

#include "../common/common.hpp"

constexpr std::array kCardsOrder{'A', 'K', 'Q', 'J', 'T', '9', '8', '7', '6', '5', '4', '3', '2'};

struct Hand
{
    std::string cards;
    std::int64_t bid;
    std::int64_t rank;
};

auto card_value(auto card) noexcept
{
    return std::distance(kCardsOrder.begin(), std::ranges::find(kCardsOrder, card));
}

auto hand_rank(const std::vector<char>& hand, const std::vector<std::int64_t>& seq) noexcept
{
    if (seq[0] == 5) return 1;
    if (seq[0] == 4 || seq[1] == 4) return 2;
    if (std::ranges::find(seq, 3) != seq.end()) {
        if (std::ranges::find(seq, 2) != seq.end()) return 3;
        return 4;
    }
    if (std::ranges::count(seq, 2) == 2) return 5;
    if (*std::ranges::max_element(seq) == 1) return 7;
    return 6;
}

auto check_duplicates(std::span<const char> hand) noexcept
{
    return std::distance(hand.begin(), std::ranges::find_if(hand, [first = hand[0]](auto c) { return c != first; }));
}

auto card_seq(std::span<const char> hand) noexcept
{
    std::vector<std::int64_t> seq;
    seq.reserve(hand.size());
    std::int64_t index{0};
    while (index < hand.size()) {
        auto count = check_duplicates(hand.subspan(index));
        seq.push_back(count);
        index += count;
    }
    return seq;
}

auto parse_hand(std::string_view line) noexcept
{
    auto card_sep = line.find_first_of(' ');
    assert(card_sep != std::string::npos);

    auto cards_str = line.substr(0, card_sep);
    std::vector<char> cards;
    cards.reserve(cards_str.size());
    for (auto c : cards_str) {
        cards.push_back(c);
    }
    std::ranges::sort(cards, [](auto lhs, auto rhs) { return card_value(lhs) < card_value(rhs); });
    auto seq = card_seq(cards);
    auto rank = hand_rank(cards, seq);
    return Hand{.cards = std::string{cards_str}, .bid = std::stoll(line.substr(card_sep).data()), .rank = rank};
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Missing input!" << std::endl;
        return 1;
    }

    std::ifstream input_file(argv[1], std::ios_base::in);

    std::vector<Hand> hands;

    std::string line;
    while (std::getline(input_file, line)) {
        hands.push_back(parse_hand(line));
    }

    std::ranges::sort(hands, [](const auto& lhs, const auto& rhs) {
        if (lhs.rank != rhs.rank) return lhs.rank > rhs.rank;
        for (std::int64_t i = 0; i < lhs.cards.size(); ++i) {
            auto lhs_value = card_value(lhs.cards[i]);
            auto rhs_value = card_value(rhs.cards[i]);
            if (lhs_value == rhs_value) continue;
            return lhs_value > rhs_value;
        }
        return false;
    });

    for (auto& hand : hands) {
        for (auto c : hand.cards) {
            std::cout << c << ",";
        }
        std::cout << "|";
        std::cout << hand.bid;
        std::cout << "|";
        std::cout << hand.rank;
        std::cout << "|";
        std::cout << std::endl;
    }

    std::int64_t result_part_one{0};
    for (std::int64_t i = 0; i < hands.size(); ++i) {
        result_part_one += hands[i].bid * (i + 1);
    }
    std::cout << result_part_one << std::endl;
    return 0;
}

// 251058093
