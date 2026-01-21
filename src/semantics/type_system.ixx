module;
#include <memory>

export module type_system;
import ast;

namespace tc {
using ET = Expression_type;


ET deduce_type(Expression& expr) {

}

ET promote_type(Expression& expr) {
    
}

bool convertable_to(ET from, ET to) {

}

void add_implicit_conversion(std::unique_ptr<Expression>& expr, ET type) {

}

ET common_type(ET t1, ET t2) {

}


}

