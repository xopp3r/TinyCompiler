module;
#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>

export module fcm;
import vocabulary;

namespace fsm {

template <typename T, size_t child_num> class Fsm_state {
  public:
    std::optional<T> value;

    std::optional<Fsm_state &>
    get_child(char index) const {
        return *childrens[static_cast<unsigned char>(index)];
    }

    void
    assign_child(char index, Fsm_state &&new_val) {
        childrens[static_cast<unsigned char>(index)] = std::make_unique(new_val);
    };

  private:
    std::array<std::unique_ptr<Fsm_state>, child_num> childrens{};
};

template <typename T, Translation_table lookup_table> class Parsing_fsm {
  public:
    using State = Fsm_state<T, lookup_table.size()>;

    void
    insert(std::string_view key, T value) {
        State *current = root.get(); // FIXME ugly raw pointers
        for (char c : key) {
            auto node = current->get_child(c);
            if (not node.has_value()) {
                current->assign_child(c, State{});
            }
            current = &node->value(); // FIXME
        }
        current->value = value;
    }

    bool
    insert_node(std::string_view key, State &&state) {
        auto previous_node = find_node(key.subview(0, std::size(key) - 1));
        char last = key.at(std::size(key) - 1);
        if (previous_node->get_child(last)) {
            return false;
        }
        previous_node->assign_child(last, state);
    }

    template <typename R> // TODO
    bool
    erase(std::string_view key) {
        static_assert(false && key.empty(), "Unimplemented");
    };

    std::optional<T &>
    find(std::string_view key) const {
        return find_node(key).and_then([](auto &&val) { return val.value(); }).or_else([]() { return std::nullopt; });
    }

  private:
    std::unique_ptr<State> root{State{}};

    std::optional<State &>
    find_node(std::string_view key) const {
        State *current = root.get(); // FIXME ugly raw pointers
        for (char c : key) {
            auto node = current->get_child(c);
            if (not node.has_value()) {
                return std::nullopt;
            }
            current = &node->value(); // FIXME
        }
        return *current;
    };
};

} // namespace fsm
