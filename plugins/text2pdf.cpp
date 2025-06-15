#include <hpdf.h>
#include "text2pdf.hpp"
std::string Text2Pdf::support_mime_type() const {
    return "text/plain";
}
std::string Text2Pdf::target_format() const {
    return "pdf";
};
extern "C" IConverterFactory* create_plugin() {
    return new Text2Pdf();
}