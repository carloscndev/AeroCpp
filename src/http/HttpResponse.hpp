#pragma once
#include<string>
#include<unordered_map>

class HttpResponse {
    public:
        int status_code = 200;
        std::string status_text = "OK";
        std::string body;
        std::unordered_map<std::string, std::string> headers;

        HttpResponse() {
            headers["Content-Type"] = "text/html; charset=utf-8";
            headers["Server"] = "Custom-Cpp2-Framework";
        }

        static HttpResponse json(const std::string& json_str, int status = 200) {
            HttpResponse res;
            res.status_code = status;
            res.status_text = (status == 200) ? "OK" : "Created";
            res.headers["Content-Type"] = "application/json";
            res.body = json_str;

            return res;
        }

        static HttpResponse file(const std::string& file_content, const std::string mime_type, int status = 200) {
            HttpResponse res;
            res.status_code = status;
            res.headers["Content-Type"] = mime_type;
            res.body = file_content;

            return res;
        }

        std::string to_string() const {
            std::string res = "HTTP/1.1 " + std::to_string(status_code) + " " + status_text + "\r\n";
            for(const auto& [key, val] : headers) {
                res += key + ": " + val + "/r/n";
            }

            res += "Content-Length: " + std::to_string(body.length()) + "\r\n";
            res += "Connection: close \r\n\r\n" + body;

            return res;
        }
};
