#include"core/ServerSocket.hpp"
#include"core/Router.hpp"
#include"http/HttpRequest.hpp"
#include"http/HttpResponse.hpp"
#include<vector>
#include<string>
#include<string_view>
#include<thread>
#include<unistd.h> // For read and write
#include<iostream>

HttpResponse handle_home(const HttpRequest& req) {
    // For this test returning a quick string response
    HttpResponse res;
   res.body = "<h1>Framework Test: Success!</h1><p>The routing pipeline is working.</p>";
    return res;
}

HttpResponse handle_staus(const HttpRequest& res) {
    return HttpResponse::json("{\"status\": \"success\", \"framework\": \"C++20 Personal Frameword\"}");
}

// Function to handle the client connection in a separate thread for each client
void handle_client(int client_id, const Router& router) {
    if (client_id < 0) return;

    try {

        // 1. Read the raw request from the socket
        std::vector<char> buffer(2048);
        ssize_t bytes_read = read(client_id, buffer.data(), buffer.size());

        if(bytes_read > 0) {
            std::string_view raw_request(buffer.data(), buffer.size());

            // 2. Parse the request using the HttpRequest Client
            HttpRequest req = HttpRequest::parse(raw_request);
            std::cout << "Request: " << req.method << " " << req.path << std::endl;

            // 3. Pass the parsed request to the Router to get an HttpResponse
            HttpResponse res = router.route(req);

            // 4. Serialize and send the response back to the browser
            std::string raw_response = res.to_string();
            write(client_id, raw_response.data(), raw_response.size());
        }

        shutdown(client_id, SHUT_WR);
        close(client_id);

    } catch (const std::exception& e) {
        std::cerr << "Error handling client: " << e.what() << "\n";
        close(client_id);
    }
}

int main() {
  try {

    // Instantiate the centralized application router
    Router router;

    // Resister the routers
    router.get("/", handle_home);
    router.get("/api/status", handle_staus);

    ServerSocket server(8080);

    while (true) {
        int client_id = server.accept_client();

        if (client_id >= 0) {

            // Pass the router by reference into the lambda
            std::thread([client_id, &router]() {
                handle_client(client_id, router);
            }).detach();
        }
    }
  } catch(const std::exception& e) {
    std::cerr << "Fatal Error: " << e.what() << "\n";

    return 1;
  }

  return 0;
}
