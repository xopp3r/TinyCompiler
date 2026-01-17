module;
#include <format>

export module position;

namespace tc {

export struct Position {
    unsigned int offset{};
    unsigned short line{};
    unsigned short column{};
};

}  // namespace tc

template <>
struct std::formatter<tc::Position> {
    constexpr auto parse(std::format_parse_context &ctx) { return ctx.begin(); }

    constexpr auto format(const tc::Position &pos, auto &ctx) const {
        return std::format_to(ctx.out(), "[{}:{}:{}]", pos.line, pos.column, pos.offset);
    }
};
