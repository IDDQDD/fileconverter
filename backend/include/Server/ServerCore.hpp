#pragma once
#include <memory>
#include <boost/asio/ip/tcp.hpp>
#include "Server/Settings.hpp"
#include "Server/PluginManager.hpp"
#include "Server/RequestHandler.hpp"

class ServerCore : public std::enable_shared_from_this<ServerCore> {
    const ServerSettings   &server_settings_;        
    const ConnectionSettings &connection_settings_;             
    std::shared_ptr<PluginManager> plugin_manager_; 
    tcp::acceptor    acceptor_;               
    net::io_context& ioc_;                    
    std::shared_ptr<RequestHandler> request_handler_; 

public:
    ServerCore(net::io_context &ioc, const ServerSettings &settings,
               const ConnectionSettings &conn_settings,
               std::shared_ptr<PluginManager> pm);
    void run();

private:
        void accept_connections(); // Implementation for listening to incoming connections
        void handle_accept(beast::error_code ec, tcp::socket socket); 
}; // Base class for server core functionality
