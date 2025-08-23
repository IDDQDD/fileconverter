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
#include "PluginManager.hpp"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;
namespace json = boost::json;           // from <boost/json.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class ServerCore : public std::enable_shared_from_this<ServerCore> {
    net::io_context& ioc_;                    
    tcp::acceptor    acceptor_;               
    ServerSettings   server_settings_;        
    ConnectionSettings settings_;             
    std::shared_ptr<PluginManager> plugin_manager_; 
    std::shared_ptr<RequestHandler> request_handler_; 

public:
    ServerCore(net::io_context &ioc, const json::value &settings, std::shared_ptr<PluginManager> pm);
    void run();

private:
        void accept_connections(); // Implementation for listening to incoming connections
        void handle_accept(beast::error_code ec, tcp::socket socket); 
}; // Base class for server core functionality
