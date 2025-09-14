#pragma once
#include <shared_mutex>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include "Server/Settings.hpp"
#include "Server/PluginManager.hpp"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;
namespace json = boost::json;           // from <boost/json.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

    enum class State {
        WaitingJson,
        WaitingFile,
        Converting,
        Done,
    };

class RequestHandler : std::enable_shared_from_this<RequestHandler> {

    websocket::stream<tcp::socket> ws_;
    const ConnectionSettings &settings_; // Settings for the connection
    State state_;
    std::shared_ptr<PluginManager> plugin_manager_;
    beast::flat_buffer buffer_; // Buffer for reading data
    IConverterFactory *converter; // Pointer to the converter factory for processing data
    std::unique_ptr<const md::Metadata> metadata_; // Metadata for the request
    std::optional<ConvertResult> result_;
    public:

        RequestHandler() = delete; // Delete default constructor to prevent instantiation without parameters
        explicit RequestHandler(tcp::socket&& socket, const ConnectionSettings &settings, 
                                std::shared_ptr<PluginManager> plugin_manager);
        void handle_request();
    
    private:
        
        void set_settings();
        void read_request();
        void send_response(const std::optional<ConvertResult>& result, std::size_t size);
        void process_request(beast::error_code ec, std::size_t bytes_transferred);
        std::optional<std::string> detect_mime_type(std::size_t bytes_transferred_);
        bool is_valid_mime_type(std::optional<std::string> mime_type);

        void send_response(const std::optional<ConvertResult>& result, bool close_on_finish,
                           websocket::close_code close_code);

        void handle_json(std::size_t &bytes_transferred);
        void handle_file(std::size_t &bytes_transferred);
    }; // Class for handling HTTP requests
// This class is responsible for reading incoming requests, processing them, and sending responses back to the client.