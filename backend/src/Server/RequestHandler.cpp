#include <magic.h>
#include <../include/Server/ErrorHandler.hpp>
#include <../include/Server/RequestHandler.hpp>
#include "RequestHandler.hpp"

RequestHandler::RequestHandler(tcp::socket &&socket, const ConnectionSettings &settings,
                               std::shared_ptr<PluginManager> plugin_manager)
     : ws_(std::move(socket)), settings_(&settings), state_(State::WaitingJson), 
       plugin_manager_(plugin_manager){};
 
void RequestHandler::handle_request() {
    ws_.async_accept(
        [self = shared_from_this()](beast::error_code ec) {
            if(ec) {
                ErrorHandler::log_to_file(ec.message());
            } else {
                self->set_settings();
                self->read_request();
            }
        }
    );
}

void RequestHandler::set_settings() {
    // Set connection settings for the request
    // settings_.set_default();
    ws_.auto_fragment(settings_->auto_fragment);
    ws_.read_message_max(settings_->read_message_max);
}

void RequestHandler::read_request() {
    // Read the incoming request into the buffer
    ws_.async_read(buffer_, beast::bind_front_handler(
                   &RequestHandler::process_request, shared_from_this()));
    
}

void RequestHandler::process_request(beast::error_code ec, std::size_t bytes_transferred) {
    // Process the request from the buffer
    // This is where you would parse the request and prepare a response
    // For now, we will just log_to_file the received data
    if(ec) {
        ErrorHandler::log_to_file("Error reading request: " + ec.message());
        return;
    }
    if(state_ == State::WaitingJson) {
        handle_json(bytes_transferred);
        state_ = State::WaitingFile;
        read_request(); // Continue reading the next request
        return;
    } if(state_ == State::WaitingFile) {
        handle_file(bytes_transferred);     
    }
    
        
}
std::optional<std::string> RequestHandler::detect_mime_type(std::size_t bytes_transferred_) {
    auto magic = magic_open(MAGIC_MIME_TYPE);
    if(!magic){
        ErrorHandler::log_to_file("Could not initialize libmagic ");
        return std::nullopt;
    }
    if(magic_load(magic, nullptr)) {
        ErrorHandler::log_to_file("Could not load magic database");
        return std::nullopt; // Need error handler, which send a response to the client
    }
    auto mime = magic_buffer(magic,buffer_.data().data(),
                             bytes_transferred_);
    magic_close(magic);
return mime;
}
void RequestHandler::send_response(const std::optional<ConvertResult>& result, bool close_on_finish = false,
                                   websocket::close_code code = websocket::close_code::normal) {
// Send a response back to the client
    if(result) {
        ws_.async_write(boost::asio::buffer(result->data.data(), result->size),
            [self = shared_from_this(), close_on_finish, code](beast::error_code ec) {
                if(ec) {
                ErrorHandler::log_to_file("Error sending response: " + ec.message());
                return;
            } else if(close_on_finish) {
                // Successfully sent the response, you can read the next request if needed
                self->ws_.async_close(code,
                    [self](beast::error_code ec) {
                        if(ec) {
                            ErrorHandler::log_to_file("Error closing connection: " + ec.message());
                        }
                    });
            } else self->read_request();   
        
        });

}
                                   }

void RequestHandler::handle_json(std::size_t &bytes_transferred) {
    auto message = beast::buffers_to_string(buffer_.data());
    metadata_ = std::make_unique<Metadata>(json::parse(message));
    if(metadata_->mime_type.empty() || metadata_->target_format.empty()){
        ErrorHandler::log_to_file("Invalid metadata received");
        send_response(std::nullopt, true, websocket::close_code::protocol_error);
    }
    ws_.text(false);
    buffer_.consume(bytes_transferred);
};

void RequestHandler::handle_file(std::size_t &bytes_transferred) {
    auto plugin = plugin_manager_->get_converter(metadata_->mime_type, metadata_->target_format);

    if(!plugin) {
        ErrorHandler::log_to_file("No suitable plugin found for " + metadata_->mime_type + " to " + metadata_->target_format);
        send_response(std::nullopt, true, websocket::close_code::unknown_data);
        return;
    }
    auto result = plugin->convert(buffer_.data().data(), bytes_transferred);
    if(!result) {
        ErrorHandler::log_to_file("Conversion failed or unsupported format");
        send_response(std::nullopt, true, websocket::close_code::unknown_data);
        return;
    }
    send_response(result, true, websocket::close_code::normal);
    buffer_.consume(bytes_transferred);
    ws_.text(true);
};

bool RequestHandler::is_valid_mime_type(std::optional<std::string> mime_type){
    if(!mime_type.has_value()){
        return false;
    } else {
      return mime_type.value() == metadata_->mime_type;
    }
}