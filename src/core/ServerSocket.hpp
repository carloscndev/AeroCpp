#pragma once
#include<iostream>
#include<stdexcept>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>

class ServerSocket {
private:
  int server_id; //Main server socket descriptor

public:
  // Constructor: Initialize the socket, binds it, ans starts listening
ServerSocket(int port) {
    // 1. Create the socket
    server_id = socket(AF_INET, SOCK_STREAM, 0);
    if (server_id < 0) {
      throw std::runtime_error("Failed to create socket");
    }

    // Allow rapid reuse of the port after restarting the server
    int opt = 1;
    setsockopt(server_id, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. Configure the address structure
    sockaddr_in address{};
    address.sin_family = AF_INET; // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Listen on any network interface
    address.sin_port = htons(port); // Convert port to network byte order

    if(bind(server_id, (struct sockaddr*)&address, sizeof(address)) < 0) {
      throw std::runtime_error("Bind failed (port might be in use)");
    }

    // 3. Start listening for incoming connections
    // 32 is the maximum number of pending connections in the queue
    if (listen(server_id, 32) < 0) {
      throw std::runtime_error("Listen failed");
    }

    std::cout << "Server is up and listening on port: " << port << " ...\n";
  }

  // Destructor
  ~ServerSocket() {
    if (server_id >= 0) {
      close(server_id);
    }
  }

  // Block and wait for a client to connect
  int accept_client() {
    sockaddr_in client_address{};
    socklen_t client_len = sizeof(client_address);

    // This block execution until someone visit http://localhost:8080
    int client_id = accept(server_id, (struct sockaddr*)&client_address, &client_len);
    if (client_id < 0){
      std::cerr << "Warning: Failed to accept client connection\n";
    }
    return client_id;
  }
};

