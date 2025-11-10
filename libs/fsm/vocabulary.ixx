module;
#include <array>
#include <cstddef>
#include <limits>

export module vocabulary;

export namespace fsm {
constinit unsigned char INVALID = std::numeric_limits<unsigned char>::max();
}

template <size_t length>
consteval auto
inverse(std::array<unsigned char, length> in) {
    std::array<unsigned char, std::numeric_limits<unsigned char>::max()> result;

    for (size_t i = 0; i < std::size(in); i++) {
        result[i] = fsm::INVALID;
    }
    for (size_t i = 0; i < std::size(in); i++) {
        result[in[i]] = i;
    }

    return result;
}

export namespace fsm {

template <size_t length> class Translation_table {
  public:
    Translation_table() = delete;

    constexpr Translation_table(std::array<unsigned char, length> &alphabet)
        : to_external(alphabet), to_internal(inverse(alphabet)) {
        static_assert(length < std::numeric_limits<unsigned char>::max(), "Aplhabet is too big");
    }

    // constexpr unsigned char
    // operator[](unsigned char in) const {
    //     return internal_representation(in);
    // }

    constexpr unsigned char
    internal_representation(unsigned char in) const {
        return to_internal[static_cast<size_t>(in)];
    }

    constexpr unsigned char
    external_representation(unsigned char in) const {
        return in < length ? to_external[static_cast<size_t>(in)] : INVALID;
    }

    constexpr size_t
    size() const {
        return length;
    }

  private:
    std::array<unsigned char, length> to_external{};
    std::array<unsigned char, std::numeric_limits<unsigned char>::max()> to_internal{};
};

} // namespace fsm
