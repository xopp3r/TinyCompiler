module;
#include <algorithm>
#include <concepts>
#include <cstddef>
#include <optional>
#include <utility>

export module DFA;

namespace dfa {

template <typename F, typename State, typename Symbol>
concept transition_function = requires(F f, State s, Symbol sym) {
    { std::invoke(f, s, sym) } -> std::convertible_to<State>;
};

template <typename F, typename State, typename Output>
concept accepting_state = requires(F f, State s) {
    { std::invoke(f, s) } -> std::convertible_to<Output>;
};

export template <typename Symbol, typename State, transition_function<State, Symbol> Trans_func, typename Output,
                 accepting_state<State, Output> Accept_func>
class DFA {
   public:

    constexpr DFA(Accept_func&& presentation_func, State initial_state, Trans_func&& transition_func,
                  State invalid_state, Output invalid_output, Symbol)
        : current_state(initial_state),
          initial_state(initial_state),
          invalid_state(invalid_state),
          invalid_output(invalid_output),
          transition_func(std::move(transition_func)),
          presentation(std::move(presentation_func)) {}

    constexpr void reset() noexcept { current_state = initial_state; }

    // check try_step method before
    constexpr std::optional<Output> step(Symbol s) noexcept {
        current_state = transition_func(current_state, s);
        Output o = presentation(current_state);
        return o == invalid_output ? std::nullopt : std::optional{o};
    }

    constexpr bool try_step(Symbol s) const noexcept { return invalid_state != transition_func(current_state, s); }

   private:
    State current_state;

    const State initial_state;

    const State invalid_state;
    const Output invalid_output;

    const Trans_func transition_func;
    const Accept_func presentation;
};


template<typename Accept_func, typename Trans_func, typename State, 
         typename Output, typename Symbol>
DFA(Accept_func&&, State, Trans_func&&, State, Output, Symbol)
    -> DFA<Symbol, State, Trans_func, Output, Accept_func>;

}  // namespace dfa
