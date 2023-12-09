
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

constexpr auto kJoker = 'J';
constexpr std::array kCardsOrder{'A', 'K', 'Q', 'T', '9', '8', '7', '6', '5', '4', '3', '2', kJoker};

auto card_value(char card) noexcept
{
    return std::distance(kCardsOrder.begin(), std::ranges::find(kCardsOrder, card));
}

struct CardSeq
{
    std::int64_t count;
    char card;

    auto operator==(const CardSeq&) const noexcept -> bool = default;
};

auto card_seq_comparison(const CardSeq& lhs, const CardSeq& rhs) noexcept
{
    if (lhs.count != rhs.count) return lhs.count > rhs.count;
    return card_value(lhs.card) > card_value(rhs.card);
}

auto hand_rank(std::span<const char> sorted_cards, std::span<const CardSeq> seq) noexcept
{
    auto joker_count = std::ranges::count(sorted_cards, kJoker);

    if (seq[0].count + joker_count >= 5) return 1;
    if (seq[0].count + joker_count >= 4) return 2;
    if (seq[0].count + joker_count == 3) {
        if (seq[1].count == 2) return 3;
        return 4;
    }
    if (std::ranges::count(seq, 2, &CardSeq::count) == 2) return 5;
    if (seq[0].count + joker_count == 2) return 6;
    return 7;
}

auto check_card_sequence(std::span<const char> sorted_cards) noexcept -> std::int64_t
{
    auto first = sorted_cards[0];
    if (first == kJoker) return 1;
    auto seq_end = std::ranges::find_if(sorted_cards, [&first](auto c) { return c != first; });
    return std::distance(sorted_cards.begin(), seq_end);
}

auto card_seq(std::span<const char> sorted_cards) noexcept
{
    std::vector<CardSeq> seq;
    seq.reserve(sorted_cards.size());
    std::int64_t index{0};
    while (index < sorted_cards.size()) {
        auto count = check_card_sequence(sorted_cards.subspan(index));
        seq.push_back({.count = count, .card = sorted_cards[index]});
        index += count;
    }
    std::ranges::sort(seq, card_seq_comparison);
    return seq;
}

struct Hand
{
    std::string cards;
    std::int64_t bid;
    std::int64_t rank;
};

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

    // for (auto& hand : hands) {
    //     for (auto c : hand.cards) {
    //         std::cout << c << ",";
    //     }
    //     std::cout << "|";
    //     std::cout << hand.bid;
    //     std::cout << "|";
    //     std::cout << hand.rank;
    //     std::cout << "|";
    //     std::cout << std::endl;
    // }

    std::int64_t result{0};
    for (std::int64_t i = 0; i < hands.size(); ++i) {
        result += hands[i].bid * (i + 1);
    }
    std::cout << result << std::endl;
    return 0;
}
