#include <iostream>
#include <ranges>
import driver;

auto main() -> int {
    auto source = std::ranges::subrange(std::istreambuf_iterator<char>(std::cin), std::default_sentinel);
    tc::compile(source, true);
}
