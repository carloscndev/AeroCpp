#pragma once
#include "../http/HttpRequest.hpp"
#include "../http/HttpResponse.hpp"
#include <optional>
#include <functional>

class IMiddleware {
public:
    // Virtual destructor ensures derived class resources are cleaned up safely
    virtual ~IMiddleware() = default;

    // If it returns an HttpResponse, it breaks the pipeline (Early Return, e.g., 401 Unauthorized)
    // If it returns std::nullopt, the execution continues to the next step
    virtual std::optional<HttpResponse> execute(const HttpRequest& req) = 0;
};
