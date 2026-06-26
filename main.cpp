#include "ServerSocket.hpp"
#include<vector>
#include<string>
#include<string_view>
#include <unistd.h> // For read and write

int main() {
  
  try {
    ServerSocket server(8080);

    while (true) {
      int client_id = server.accept_client();

      if(client_id >= 0) {
        std::cout << "\n Client connected! 'n";
        
        // 1. Read the incoming HTTP request from browser
        std::vector<char> buffer(2048);
        ssize_t bytes_read = read(client_id, buffer.data(), buffer.size());

        if(bytes_read > 0) {
          std::cout << "---Received Request ---\n";
          std::cout << std::string_view(buffer.data(), bytes_read) << "\n";
          std::cout << "-----------------------\n";
        }

        // 2. WRITE a valid HTTP response back to client
        std::string html_content = "<h1>Hello from C++ 20 Server</h1>";

        // Raw HTTP response format (CRLF "\r\n" is mandatory)
        std::string http_response =
          "HTTP/1.1 200 OK\r\n"
          "Content-Type: text/html; charset=utf-8\r\n"
          "Content-Leng: " + std::to_string(html_content.length()) + "\r\n"
          "Connection: Close\r\n"
          "\r\n" + // Mandatory empty line separating headers from body
          html_content;

        write(client_id, http_response.data(), http_response.size());

        // 3. Close the connection with the client
        shutdown(client_id, SHUT_WR);
        close(client_id);
      }
    }
  } catch(const std::exception& e) {
    std::cerr << "Fatal Error: " << e.what() << "\n";
    
    return 1;
  }

  return 0;
}