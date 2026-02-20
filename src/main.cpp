#include <iostream>
#include <ranges>
import driver;

auto main() -> int {
    auto source = std::views::istream<char>(std::cin);
    tc::compile(source, true);
}
