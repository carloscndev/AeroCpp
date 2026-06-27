#pragma once
#include<string>
#include<string_view>
#include<unordered_map>
#include<sstream>

class HttpRequest {
    public:
        std::string method;
        std::string path;
        std::string version;
        std::unordered_map<std::string, std::string> headers;

    static HttpRequest parse(std::string_view raw_request) {
        HttpRequest req;
        std::string req_str(raw_request); // Convert to streamable string
        std::istringstream stream(req_str);
        std::string line;

        // 1. Parse Request line (e.g. "GET /public/index.html HTTP/1.1")
        if (std::getline(stream, line)) {
            // Remove trailing '\r'
            if(!line.empty() && line.back() == '\r') line.pop_back();

            std::istringstream line_stream(line);
            line_stream >> req.method >> req.path >> req.version;
        }

        // 2. Parse Headers
        while (std::getline(stream, line)) {
            if(!line.empty() && line.back() == '\r') line.pop_back();
            if(line.empty()) break;

            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string key = line.substr(0, colon);
                std::string value = line.substr(colon + 1);

                // Trim Space

                value.erase(0, value.find_first_not_of(" "));
                req.headers[key] = value;
            }
        }

        return req;
    }
};
