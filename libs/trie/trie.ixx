module;
#include <cstddef>
#include <memory>
#include <optional>
#include <string_view>

export module trie;
export import compression_table;

namespace trie {

template <typename T, size_t child_num> 
class Trie_node {
  public:
    std::optional<T> value;

    Trie_node* get_child(unsigned char index) {
        return m_childrens[index].get();
    }

    const Trie_node* get_child(unsigned char index) const {
        return m_childrens[index].get();
    }

    void assign_child(unsigned char index, std::unique_ptr<Trie_node> new_child) {
        m_childrens[index] = std::move(new_child);
    }

    Trie_node& emplace_child(unsigned char index) {
        m_childrens[index] = std::make_unique<Trie_node>();
        return *m_childrens[index];
    }

  private:
    std::array<std::unique_ptr<Trie_node>, child_num> m_childrens{};
};

export template <typename T, Compression_table lookup_table> 
class Trie {
  public:
    using node_type = Trie_node<T, lookup_table.size()>;
    using value_type = T;

    bool insert(std::string_view key, T value) {
        node_type* current = m_root.get();
        for (const char c : key) {
            const unsigned char idx = lookup_table.internal_representation(c);
            if (!current->get_child(idx)) {
                current->emplace_child(idx);
            }
            current = current->get_child(idx);
        }
        
        const bool inserted = !current->value.has_value();
        current->value = std::move(value);
        return inserted;
    }

    T* find(std::string_view key) {
        node_type* node = find_node(key);
        return node ? &node->value.value() : nullptr;
    }

    const T* find(std::string_view key) const {
        const node_type* node = find_node(key);
        return node ? &node->value.value() : nullptr;
    }

    bool contains_prefix(std::string_view key_prefix) const {
        return find_node(key_prefix) != nullptr;
    }

    bool contains(std::string_view key) const {
        return find(key) != nullptr;
    }

  private:
    std::unique_ptr<node_type> m_root{std::make_unique<node_type>()};

    node_type* find_node(std::string_view key) {
        node_type* current = m_root.get();
        for (const char c : key) {
            const unsigned char idx = lookup_table.internal_representation(c);
            current = current->get_child(idx);
            if (!current) return nullptr;
        }
        return current;
    }

    const node_type* find_node(std::string_view key) const {
        const node_type* current = m_root.get();
        for (const char c : key) {
            const unsigned char idx = lookup_table.internal_representation(c);
            current = current->get_child(idx);
            if (!current) return nullptr;
        }
        return current;
    }
};


} // namespace trie
