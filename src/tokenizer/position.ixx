module;

#include <format>

export module position;

namespace tc {

export struct Position {
    size_t line{};
    size_t column{};
    size_t offset{};
};

}  // namespace tc

template <>
struct std::formatter<tc::Position> {
    constexpr auto parse(std::format_parse_context &ctx) const noexcept { return ctx.begin(); }

    constexpr auto format(const tc::Position &pos, std::format_context &ctx) const {
        return std::format_to(ctx.out(), "{}:{}:{}", pos.line, pos.column, pos.offset);
    }
};

template <>
struct std::formatter<tc::Position&> : std::formatter<tc::Position> {};

template <>
struct std::formatter<const tc::Position&> : std::formatter<tc::Position> {};
