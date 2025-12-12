module;
#include <algorithm>
#include <array>
#include <concepts>
#include <cstddef>
#include <optional>
#include <utility>

export module DFA;

namespace dfa {

constexpr auto binary_search_range(std::ranges::random_access_range auto&& range, const auto& value, auto proj) {
    const auto it = std::ranges::lower_bound(range, value, {}, proj);
    return (it != std::ranges::end(range) && *it == value) ? it : std::ranges::end(range);
}

template <typename F, typename State, typename Symbol>
concept Transition_function = requires(F f, State s, Symbol sym) {
    { std::invoke(f, s, sym) } -> std::convertible_to<State>;
};

template <typename State, size_t states_cnt, size_t final_states_cnt, typename Symbol, size_t alphabet_size,
          Transition_function<State, Symbol> Func, typename Output>
class DFA {
   public:
    constexpr DFA(const std::array<State, states_cnt>& states, const std::array<Symbol, alphabet_size>& alphabet,
                  const std::array<std::pair<State, Output>, final_states_cnt>& accepting_states, State initial_state,
                  Func&& transition_func, State invalid_state)
        : states(states),
          alphabet(alphabet),
          accepting_states(accepting_states),
          current_state(initial_state),
          initial_state(initial_state),
          invalid_state(invalid_state),
          transition_func(std::move(transition_func)) {}

    constexpr void reset() noexcept { current_state = initial_state; }

    // check try_step method before
    constexpr std::optional<Output> step(Symbol s) noexcept {
        current_state = transition_func(current_state, s);
        auto it = binary_search_range(accepting_states, current_state, &std::pair<State, Output>::first);
        return it == std::ranges::cend(accepting_states) ? std::nullopt : it->second;
    }

    constexpr bool try_step(Symbol s) const noexcept { return invalid_state != transition_func(current_state, s); }

   private:
    std::array<State, states_cnt> states;
    std::array<Symbol, alphabet_size> alphabet;
    std::array<std::pair<State, Output>, final_states_cnt> accepting_states;

    State current_state;
    const State initial_state;
    const State invalid_state;

    const Func transition_func;
};

}
