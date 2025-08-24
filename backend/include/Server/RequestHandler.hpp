#pragma once
#include <Server/ServerCore.hpp>
#include <shared_mutex>
#include "Settings.hpp"
#include "IConverterFactory.hpp"
#include "PluginManager.hpp"


class RequestHandler : std::enable_shared_from_this<RequestHandler> {

    enum class State {
        WaitingJson,
        WaitingFile,
        Converting,
        Done,
    };
    State state_;
    beast::flat_buffer buffer_; // Buffer for reading data
    websocket::stream<tcp::socket> ws_;
    IConverterFactory *converter; // Pointer to the converter factory for processing data
    std::shared_ptr<const ConnectionSettings>settings_; // Settings for the connection
    std::unique_ptr<const Metadata> metadata_; // Metadata for the request
    std::shared_ptr<PluginManager> plugin_manager_;
    
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
        std::string detect_mime_type(std::size_t bytes_transferred_);

        void send_response(const std::optional<ConvertResult>& result, std::size_t size, bool close_on_finish,
                           websocket::close_code close_code);

        void handle_json(std::size_t &bytes_transferred);
        void handle_file(std::size_t &bytes_transferred);
    }; // Class for handling HTTP requests
// This class is responsible for reading incoming requests, processing them, and sending responses back to the client.