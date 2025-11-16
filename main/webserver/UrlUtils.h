#pragma once
#include <string>
#include <map>
#include "esp_http_server.h"

class UrlUtils {
public:
    // Decodes a URL-encoded string (e.g., from form or query string)
    static std::string url_decode(const std::string &in) {
        std::string out; out.reserve(in.size());
        for (size_t i=0;i<in.size();++i) {
            if (in[i] == '+') { out.push_back(' '); }
            else if (in[i] == '%' && i+2 < in.size()) {
                int hi = std::toupper(in[i+1]);
                int lo = std::toupper(in[i+2]);
                auto hexval=[](int c){ if (c>='0'&&c<='9') return c-'0'; if (c>='A'&&c<='F') return c-'A'+10; return 0; };
                out.push_back(static_cast<char>(hexval(hi)*16 + hexval(lo)));
                i+=2;
            } else { out.push_back(in[i]); }
        }
        return out;
    }

    // Parses application/x-www-form-urlencoded body from POST
    static std::map<std::string,std::string> parse_urlencoded_form(httpd_req_t *req) {
        std::map<std::string,std::string> result;
        int total_len = req->content_len;
        if (total_len <= 0) {
            return result;
        }
        std::string body; body.resize(total_len);
        int received = 0;
        while (received < total_len) {
            int r = httpd_req_recv(req, &body[received], total_len - received);
            if (r <= 0) { return result; }
            received += r;
        }
        size_t start = 0;
        while (start < body.size()) {
            size_t amp = body.find('&', start);
            std::string pair = body.substr(start, amp == std::string::npos ? std::string::npos : amp - start);
            size_t eq = pair.find('=');
            if (eq != std::string::npos) {
                std::string key = pair.substr(0, eq);
                std::string val = url_decode(pair.substr(eq+1));
                result[key] = val;
            }
            if (amp == std::string::npos) break; else start = amp + 1;
        }
        return result;
    }
};
