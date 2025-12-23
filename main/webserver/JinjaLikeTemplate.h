
#pragma once
#include <string>
#include <map>

class JinjaLikeTemplate {
public:
    using VarMap = std::map<std::string, std::string>;

    // Render a template string replacing {{key}} with provided values
    static std::string render(const std::string &tmpl, const VarMap &vars) {
        std::string out = tmpl;
        for (const auto &kv : vars) {
            std::string token = "{{" + kv.first + "}}";
            size_t pos = 0;
            while ((pos = out.find(token, pos)) != std::string::npos) {
                out.replace(pos, token.size(), kv.second);
                pos += kv.second.size();
            }
        }
        return out;
    }
};
