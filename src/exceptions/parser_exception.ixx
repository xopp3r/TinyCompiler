module;
#include <exception>
#include <string>

export module parser_exception;
import position;

namespace tc {

export class Parser_exception : public std::exception {
   public:
    constexpr Parser_exception(const std::string &msg, Position pos) : pos(pos), msg(msg) {}
    constexpr Parser_exception(std::string &&msg, Position pos) : pos(pos), msg(std::move(msg)) {}

    constexpr virtual ~Parser_exception() = default;
    constexpr const char *what() const noexcept override { return msg.c_str(); }

    Position where() const noexcept { return pos; }

   private:
    Position pos;
    std::string msg;
};

}  // namespace tc