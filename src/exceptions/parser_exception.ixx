module;
#include <exception>
#include <optional>
#include <string>

export module parser_exception;
import position;

namespace tc {

export class Parser_exception : public std::exception {
   public:
    constexpr Parser_exception(std::string msg, Position pos) : msg(std::move(msg)), pos(pos) {}
    constexpr Parser_exception(std::string msg) : msg(std::move(msg)), pos(std::nullopt) {}

    constexpr virtual ~Parser_exception() = default;
    constexpr const char *what() const noexcept override { return msg.c_str(); }

    std::optional<Position> where() const noexcept { return pos; }

   private:
    std::string msg;
    std::optional<Position> pos;
};

}  // namespace tc