module;
#include <cstdlib>
#include <format>
#include <print>
#include <source_location>
#include <string>
#include <string_view>

export module diagnostics;
import position;

enum class Diagnostic_level { note, warning, error, fatal_error };

class Diagnostic {
   public:
    constexpr Diagnostic(Diagnostic_level level, std::string msg, Position position)
        : m_level{level}, m_position{position}, m_message{msg}, m_location{std::source_location::current()} {}

    Diagnostic_level level() const noexcept { return m_level; }
    std::string_view message() const noexcept { return m_message; }
    Position position() const noexcept { return m_position; }
    const std::source_location& location() const noexcept { return m_location; }

   private:
    Diagnostic_level m_level;
    Position m_position;
    std::string m_message;
    std::source_location m_location;
};

template <>
struct std::formatter<Diagnostic_level> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    constexpr auto format(const Diagnostic_level& lvl, std::format_context& ctx) const {
        std::string_view str;
        switch (lvl) {
            case Diagnostic_level::note:
                str = "note";
                break;
            case Diagnostic_level::warning:
                str = "warning";
                break;
            case Diagnostic_level::error:
                str = "error";
                break;
            case Diagnostic_level::fatal_error:
                str = "fatal error";
                break;
            default:
                str = "unknown";
                break;
        }
        return std::format_to(ctx.out(), "{}", str);
    }
};

template <>
struct std::formatter<Diagnostic> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    constexpr auto format(const Diagnostic& diagnostic, std::format_context& ctx) const {
        std::string_view color;

        switch (diagnostic.level()) {
            case Diagnostic_level::note:
                color = "\033[1;36m";  // cyan
                break;
            case Diagnostic_level::warning:
                color = "\033[1;33m";  // yellow
                break;
            case Diagnostic_level::error:
                color = "\033[1;31m";  // red
                break;
            case Diagnostic_level::fatal_error:
                color = "\033[1;35m";  // magenta
                break;
        }

        std::string_view color_reset{"\033[0m"};

        return std::format_to(ctx.out(), "{}{}\n{}\n{}{}", color, diagnostic.position(), diagnostic.level(),
                              diagnostic.message(), color_reset);
    }
};

export void note(std::string msg, Position pos) {
    Diagnostic err{Diagnostic_level::note, std::move(msg), pos};
    std::println("{}", err);
};

export void warning(std::string msg, Position pos) {
    Diagnostic err{Diagnostic_level::warning, std::move(msg), pos};
    std::println("{}", err);
};

export [[noreturn]] void error(std::string msg, Position pos) {
    Diagnostic err{Diagnostic_level::error, std::move(msg), pos};
    std::println("{}", err);
    std::exit(EXIT_FAILURE);
};

export [[noreturn]] void fatal_error(std::string msg, Position pos) {
    Diagnostic err{Diagnostic_level::fatal_error, std::move(msg), pos};
    std::println("{}", err);
    std::exit(EXIT_FAILURE);
};
