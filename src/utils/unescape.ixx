module;
#include <string>

export module unescape;

export std::string unescape(const std::string& s) {
    std::string result;
    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\\' && i + 1 < s.size()) {
            char next = s[i + 1];
            switch (next) {
                case 'n':  result.push_back('\n'); ++i; break;
                case 't':  result.push_back('\t'); ++i; break;
                case 'r':  result.push_back('\r'); ++i; break;
                case '\\': result.push_back('\\'); ++i; break;
                case '"':  result.push_back('"');  ++i; break;
                case '\'': result.push_back('\''); ++i; break;
                // TODO
                default:
                    result.push_back('\\');
                    result.push_back(next);
                    ++i;
                    break;
            }
        } else {
            result.push_back(s[i]);
        }
    }
    return result;
}