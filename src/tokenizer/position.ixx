module;

#include <format>

export module position;

export {

    struct Position {
        size_t line{};
        size_t column{};
        size_t cursor{};
    };

}

template <> struct std::formatter<Position> {
    constexpr auto
    parse(std::format_parse_context &ctx) {
        return ctx.begin();
    }

    constexpr auto
    format(const Position &pos, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "line: {} column: {} char: {}", pos.line, pos.column, pos.cursor);
    }
};
