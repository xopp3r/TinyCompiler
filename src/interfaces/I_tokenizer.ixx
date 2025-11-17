module;
#include <concepts>
#include <cstddef>
#include <optional>

export module I_tokenizer;
import token;
import position;

export namespace concepts {

template <typename T>
concept I_tokenizer = requires(T tokenizer) {
    { tokenizer.next_token() } -> std::same_as<std::optional<Token>>;
    { tokenizer.position() } -> std::same_as<Position>;
};

}  // namespace concepts
