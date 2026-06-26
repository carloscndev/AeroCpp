#pragma once
#include "../http/HttpRequest.hpp"
#include "../http/HttpResponse.hpp"
#include<optional>
#include<functional>

class IMiddleware {
    public:
        virtual ~IMiddleware = default;

        virtual std::optional<HttpResponse> execute(cont HttpRequest& req) = 0;
};
