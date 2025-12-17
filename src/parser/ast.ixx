module;
#include <memory>

export module ast;
export import :nodes;
import token;

namespace tc {

export class AST {
   public:
    AST() = delete;
    AST(std::unique_ptr<Programm> Root) : root(std::move(Root)){};
    ~AST() = default;

    std::unique_ptr<Programm> root;
};

}  // namespace tc
