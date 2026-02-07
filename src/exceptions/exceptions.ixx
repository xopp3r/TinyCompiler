module;
#include <exception>
#include <optional>
#include <string>

export module exceptions;
import position;

namespace tc {

export class Parser_exception : public std::exception {
   public:
    constexpr Parser_exception(std::string msg, Position pos) : msg(std::move(msg)), pos(pos) {}
    constexpr Parser_exception(std::string msg) : msg(std::move(msg)), pos(std::nullopt) {}

    constexpr virtual ~Parser_exception() = default;
    constexpr const char* what() const noexcept override { return msg.c_str(); }

    std::optional<Position> where() const noexcept { return pos; }

   private:
    std::string msg;
    std::optional<Position> pos;
};

export class Type_exception : public std::exception {
   public:
    constexpr Type_exception(std::string msg, Position pos) : pos(pos), msg(std::move(msg)) {}

    constexpr virtual ~Type_exception() = default;
    constexpr const char* what() const noexcept override { return msg.c_str(); }

    Position where() const noexcept { return pos; }

   private:
    Position pos;
    std::string msg;
};

export class Visibility_exception : public std::exception {
   public:
    constexpr Visibility_exception(std::string msg, Position pos) : pos(pos), msg(std::move(msg)) {}

    constexpr virtual ~Visibility_exception() = default;
    constexpr const char* what() const noexcept override { return msg.c_str(); }

    Position where() const noexcept { return pos; }

   private:
    Position pos;
    std::string msg;
};

}  // namespace tc