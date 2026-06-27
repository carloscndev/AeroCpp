#pragma once
#include "../http/HttpRequest.hpp"
#include "../http/HttpResponse.hpp"
#include "../middlewares/IMiddleware.hpp"
#include<functional>
#include<string>
#include<unordered_map>
#include<vector>
#include<memory>
#include<iostream>

class Router {
    public:
    using Handler = std::function<HttpResponse(const HttpRequest&)>;
    Router() = default;

    // Register a global middleware that will execute on EVERY single uncoming request
    void use(const std::unique_ptr<IMiddleware> middleware) {
        global_middlewares.push_back(std::move(middleware));
    }

    // Register a GET route with its handler and optional route-specific middlewares
    void get(const std::string& path, Handler handler, std::vector<std::unique_ptr<IMiddleware>> router_middlewares = {}) {
        routes["GET"][path] = RouteData{std::move(handler), std::move(router_middlewares)};
    }

    // Registers a POST route with its handler and optional route-specific middlewares
    void get(const std::string& path, Handler handler, std::vector<std::unique_ptr<IMiddleware>> router_middlewares = {}) {
        routes["POST"][path] = RouteData{std::move(handler), std::move(router_middlewares)};
    }


    // Process the request trough the middleware and routing pipeline
    HttpResponse.route(const HttpRequest& req) const {

        // 1. Verify if HTTP Method is supported/register
        auto method_it = routes.find(req.method);
        if(method_it == routes.end()) {
            return error_response(405, "Method Not Allowed");
        }

        // 2. Verify if a specific path exists (e.g. "/api/dashboard")
        auto route_it = method_it->second.find(req.path);
        if (route_it == method_it->second.end()) {
            return error_response(404, "Not Found");
        }

        // 3. EXECUTE GLOBAL MIDDLEWARES (pipeline step 1)
        for(const auto& middleware : global_middlewares) {
            auto early_response = middleware->execute(req);
            if(early_response.has_value()) {

                // Middleware blocked the request (early return patter)
                return early_response.value()
            }
        }

        // 4. Execute route-specific middlewares (pipeline step 2)
        for (const auto& middleware : target_route.middlewares) {
            auto early_response = middleware->execute(req);
            if(early_response.has_value()) {
                return early_response.value();
            }
        }

        // 5. Execute the final route handler
        try {
            return target_route.handler(req);
        } catch(const std::exception& e) {
            std::cerr << "Routing error in handler execution: " << e.what() << std::endl;
            return error_response(500, "Internal Server Error");
        }
    }

    private:
    struct RouteData {
        Handler handler;
        std::vector<std::unique_ptr<IMiddleware>> middlewares;
    }

    // Data Structure: Methods Map -> Paths Map -> Target Route Data
    // Example: route["GET"]["/dashboard"] -> { handler, route_middlewares }
    std::unordered_map<std::string, std::unordered_map<std::string, RouteData>> routes;

    std::vector<std::unique_ptr<IMiddleware>> global_middleware;

    // Helper to build standardized HTTP error response on the fly
    HttpResponse error_response(int status, const std::string& message) const {
        HttpResponse res;
        res.status_code = status;
        res.status_text = message;
        res.body = "<h1>" + std::to_string(status) + " " + message + "</h1>";

        return res;
    }
};

