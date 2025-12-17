module;
#include <optional>
#include <string_view>

export module parsing_dfa;
import :grammar;
import token;
import DFA;

namespace tc {
consteval auto get_parsing_dfa() {
    return dfa::DFA{output_function,        Parsing_state::INITIAL, transition_function,
                    Parsing_state::INVALID, Token_type::INVALID,    Symbol_type::INVALID};
}

export class Parsing_dfa {
   public:
    constexpr std::optional<Token_type> step(char c) noexcept { return dfs.step(map_char(c)); }
    constexpr bool try_step(char c) const noexcept { return dfs.try_step(map_char(c)); }
    constexpr void reset() noexcept { dfs.reset(); }

    static constexpr Token_type check_if_keyword(std::string_view lexeme) { return check_for_keyword(lexeme); }

   private:
    decltype(get_parsing_dfa()) dfs = get_parsing_dfa();
};

}  // namespace tc
