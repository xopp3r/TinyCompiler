module;
#include <memory>

export module ast;
export import :nodes;
import token;

namespace tc {

export class AST {
   public:
    std::unique_ptr<Programm> root;
};

}  // namespace tc
