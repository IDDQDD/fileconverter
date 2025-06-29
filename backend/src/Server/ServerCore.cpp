#pragma once
#include <../include/Server/ServerCore.hpp>
#include <../include/Server/ErrorHandler.hpp>
#include <../include/Server/Settings.hpp>
#include <../include/Server/RequestHandler.hpp>

void ServerCore::run() {
    // Start the IO context to begin processing asynchronous operations
    accept_connections();
}

ServerCore::ServerCore(net::io_context &ioc, const json::value &settings)
    : server_settings_(settings), settings_(settings),
      acceptor_(ioc, tcp::endpoint{net::ip::make_address(server_settings_.address), server_settings_.port}),
      ioc_(ioc) {};

void ServerCore::accept_connections() {
    acceptor_.async_accept(net::make_strand(ioc_), beast::bind_front_handler(
        &ServerCore::handle_accept, shared_from_this()));


} 

void ServerCore::handle_accept(beast::error_code ec, tcp::socket socket) {
    if(ec) {
        ErrorHandler::log_to_file("Error accepting connection: " +ec.message());
    }else {
        // Successfully accepted a connection, create a new RequestHandler for this socket
        std::make_shared<RequestHandler>(std::move(socket), settings_)->handle_request();
    }
    accept_connections(); // Continue accepting new connections
}

