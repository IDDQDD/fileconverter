#include <boost/json.hpp>
#include "Server/Settings.hpp"

std::unique_ptr<md::Metadata> md::build_metadata(const boost::json::value v){
        auto *root = &v.as_object();
        if(auto *md = root->if_contains("metadata"); md && md->is_object()){

            root = &md->as_object();
            if(auto *mime = root->if_contains("mime_type"); mime && mime->is_string()) {
                if(auto *fmt = root->if_contains("target_format"); fmt && fmt->is_string()) {
                    
                    return std::make_unique<md::Metadata>(
                                           mime->as_string().c_str(),
                                           fmt->as_string().c_str()
                                           );
                }
            }
        }
           
        return nullptr;
    }