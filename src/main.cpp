#include "core/ServerSocket.hpp"
#include<vector>
#include<string>
#include<string_view>
#include <unistd.h> // For read and write
#include<fstream>
#include<sstream>
#include<thread>

// Function to handle the client connection in a separate thread for each client
void handle_client(int client_id) {
    if (client_id < 0) return;

    std::cout << "\n Client connected on thread ID: " << std::this_thread::get_id();

    // 1. Read the incoming HTTP request from browser
    std::vector<char> buffer(2048);
    ssize_t bytes_read = read(client_id, buffer.data(), buffer.size());

    if(bytes_read > 0) {
        std::cout << "---Received Request ---\n";
        std::cout << std::string_view(buffer.data(), bytes_read) << "\n";
        std::cout << "-----------------------\n";
    }

    // 2. READ the html file from the disk drive
    std::string html_content;

    // Open the file index.html
    std::ifstream html_file("public/index.html");

    // Check if the files exist and openend correctly
    if (html_file.is_open()) {
        std::stringstream buffer_stream;
        buffer_stream << html_file.rdbuf(); // Read the file and dump it into the buffer
        html_content = buffer_stream.str(); // Convert the stream buffer into a standard string
        html_file.close();
    }  else {
        html_content = "<h1>404 Not Found</h1>";
    }

    // 3. WRITE the HTTP Response - Raw HTTP response format (CRLF "\r\n" is mandatory)
    std::string http_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html; charset=utf-8\r\n"
        "Content-Leng: " + std::to_string(html_content.length()) + "\r\n"
        "Connection: Close\r\n"
        "\r\n" + // Mandatory empty line separating headers from body
        html_content;

    write(client_id, http_response.data(), http_response.size());

    // 4. Close the connection with the client
    shutdown(client_id, SHUT_WR);
    close(client_id);

    std::cout << "Connection closed on thread ID: " << std::this_thread::get_id();
}

int main() {
  try {
    ServerSocket server(8080);
    std::cout << "Main Thread listening..." << std::endl;

    while (true) {
        int client_id = server.accept_client();

        if (client_id >= 0) {
            std::thread(handle_client, client_id).detach();
        }
    }
  } catch(const std::exception& e) {
    std::cerr << "Fatal Error: " << e.what() << "\n";

    return 1;
  }

  return 0;
}
