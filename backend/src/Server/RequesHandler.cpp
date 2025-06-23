#include <magic.h>
#include <../include/Server/ErrorHandler.hpp>
#include <../include/Server/RequestHandler.hpp>
#include "RequestHandler.hpp"

RequestHandler::RequestHandler(tcp::socket &&socket, const ConnectionSettings &settings)
     : ws_(std::move(socket)), settings_(&settings), state_(State::WaitingJson){};
 
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
std::string RequestHandler::detect_mime_type(std::size_t bytes_transferred_) {
    auto magic = magic_open(MAGIC_MIME_TYPE);
    if(!magic_load(magic, nullptr)) {
        ErrorHandler::log_to_file("Could not load magic database");
        return; // Need error handler, which send a response to the client
    }
    auto mime = magic_buffer(magic,buffer_.data().data(),
                             bytes_transferred_);
return mime;
}
void RequestHandler::send_response(const void* data, std::size_t size, bool close_on_finish = false,
                                   websocket::close_code code = websocket::close_code::normal) {
// Send a response back to the client
    ws_.async_write(boost::asio::buffer(data, size),
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

void RequestHandler::handle_json(size_t &bytes_transferred) {
    auto message = beast::buffers_to_string(buffer_.data());
    metadata_ = std::make_unique<Metadata>(json::parse(message));
    ws_.text(false);
    buffer_.consume(bytes_transferred);
};

std::string RequestHandler::handle_file(size_t &bytes_transferred) {
    ws_.text(true);

    buffer_.consume(bytes_transferred);
    return  "";
};
