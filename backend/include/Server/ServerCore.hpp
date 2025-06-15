#pragma once
#include <memory>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "Server/Settings.hpp"
#include "RequestHandler.hpp"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;
namespace json = boost::json;           // from <boost/json.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class ServerCore : std::enable_shared_from_this<ServerCore> {

    tcp::acceptor acceptor_;
    net::io_context &ioc_; // IO context for asynchronous operations
    ConnectionSettings settings_; // Connection settings for the server
    ServerSettings server_settings_; // Server settings for configuration
    std::shared_ptr<RequestHandler> request_handler_; // Handler for processing requests

public:
    ServerCore(net::io_context &ioc, const json::value &settings);
    void run();

private:
        void accept_connections(); // Implementation for listening to incoming connections
        void handle_accept(beast::error_code ec, tcp::socket socket); 
}; // Base class for server core functionality
