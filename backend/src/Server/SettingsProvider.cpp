#include "Server/SettingsProvider.hpp"
#include "Server/ErrorHandler.hpp"
namespace json = boost::json;


ErrorCode SettingsProvider::settings_provider(std::string filename) {
    const auto settings = read_settings_from_json_file(filename);
    if(!settings.has_value() || !settings->is_object()){
            ErrorHandler::log("Failed to read settings file.");
            return ErrorCode::FAILURE;
    }
    const auto &root = settings->as_object();

    const json::object *srv_obj = &root;
    
    if(auto *srv = srv_obj->if_contains("server"); srv && srv->is_object()) {
        srv_obj = &srv->as_object();
    }else {
        ErrorHandler::log("Missing 'server' section.");
        return ErrorCode::FAILURE;
    }

    const json::object *conn_obj = &root;

    if(auto *cn = conn_obj->if_contains("connection"); cn && cn->is_object()) {
        conn_obj = &cn->as_object();
    }else {
        ErrorHandler::log("Missing 'connection' section.");
        return ErrorCode::FAILURE;
    }

    const json::object *err_obj = &root;

     if(auto *eh = err_obj->if_contains("ErrorHandling"); eh && eh->is_object()) {
         err_obj = &eh->as_object();
     }else {
        ErrorHandler::log("Missing 'ErrorHandling' section.");
        return ErrorCode::FAILURE;
     }

    if(set_error_handling_settings(err_obj) == ErrorCode::FAILURE ||
       set_server_settings(srv_obj) == ErrorCode::FAILURE ||
       set_connection_settings(conn_obj) == ErrorCode::FAILURE){
        return ErrorCode::FAILURE;
       }

    server_settings_ = ServerSettings(json::value(*srv_obj));
    connection_settings_ = ConnectionSettings(json::value(*conn_obj));
    ErrorHandler::set_log_file(err_obj->at("log_file").as_string().c_str());
    return ErrorCode::SUCCESS;
}

std::optional<json::value> SettingsProvider::read_settings_from_json_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        ErrorHandler::log("Could not open settings file: " + filename);
        return std::nullopt;
    }

    boost::system::error_code ec;
    auto result = json::parse(file, ec);
    if (ec) {
        ErrorHandler::log("JSON parse error in " + filename + ": " + ec.message());
        return std::nullopt;
    }
    return result;
}

bool SettingsProvider::has_field_in_json(const json::object *obj, std::string_view key){
    if(auto it = obj->if_contains(key); it && (it->is_string() || it->is_int64() ||
                                               it->is_uint64() || it->is_bool())) {
        return true;
    }
    return false;
}



const ConnectionSettings& SettingsProvider::get_connection_settings() const {
    return connection_settings_;
}
const ServerSettings& SettingsProvider::get_server_settings() const {
    return server_settings_;
}

  ErrorCode SettingsProvider::set_server_settings(const json::object *serv_obj) {

        if(!has_field_in_json(serv_obj, "address") ||

        !has_field_in_json(serv_obj, "port") ||
    
        !has_field_in_json(serv_obj, "host") ||

        !has_field_in_json(serv_obj, "threads")) {

            return ErrorCode::FAILURE;
        }


    return ErrorCode::SUCCESS;
  }
  ErrorCode SettingsProvider::set_connection_settings(const json::object *conn_obj) {

        if(!has_field_in_json(conn_obj, "read_message_max") ||
           !has_field_in_json(conn_obj, "auto_fragment")) {
            return ErrorCode::FAILURE;
        }

    return ErrorCode::SUCCESS;
}
  ErrorCode SettingsProvider::set_error_handling_settings(const json::object *err_obj) {

      

        if(!has_field_in_json(err_obj, "log_file")) {
            return ErrorCode::FAILURE;
        }

    return ErrorCode::SUCCESS;
  }

