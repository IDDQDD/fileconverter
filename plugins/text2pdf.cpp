#include <hpdf.h>
#include "text2pdf.hpp"
#include "../backend/include/Server/ErrorHandler.hpp"
std::string Text2Pdf::support_mime_type() const {
    return "text/plain";
}
std::string Text2Pdf::target_format() const {
    return "pdf";
};
extern "C" IConverterFactory* create_plugin() {
    return new Text2Pdf();
}

const void* Text2Pdf::convert(const void *data, std::size_t size) {
    HPDF_Doc pdf = HPDF_New(nullptr, nullptr);
    if(!pdf) {
        ErrorHandler::log_to_file("Failed to create PDF document");
        return nullptr;
    }
    HPDF_Font font = HPDF_GetFont(pdf,"Times-Roman", "StandardEncoding");
    AddTextWithMargins(pdf, static_cast<const char*>(data), size, font);
    HPDF_SaveToStream(pdf);
    HPDF_UINT stream_size = HPDF_GetStreamSize(pdf);
    HPDF_BYTE* pdf_data = new unsigned char[stream_size];
    HPDF_ReadFromStream(pdf, pdf_data, &stream_size);
    HPDF_Free(pdf);

    return pdf_data;
}
   
   

void Text2Pdf::AddTextWithMargins(HPDF_Doc pdf, const char *data, std::size_t size, HPDF_Font font, std::size_t font_size,
                                  float left, float right, float top, float bottom) {
    const char *text = data;
    while(size > 0) {
        HPDF_Page page = HPDF_AddPage(pdf);
        if(!page) {
            ErrorHandler::log_to_file("Failed to add page tp PDF");
            break;
        }
        HPDF_STATUS status = HPDF_Page_SetFontAndSize(page, font, font_size);
        if(status != HPDF_OK) {
            ErrorHandler::log_to_file("Failed to set font and size");
            break;
        }
        status = HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
        if(status != HPDF_OK) {
            ErrorHandler::log_to_file("Failed to set page size");
            break;
        }
        float page_right = HPDF_Page_GetWidth(page) - right;
        float page_bottom = HPDF_Page_GetHeight(page) - bottom;
        HPDF_UINT used_len = size;
        status = HPDF_Page_TextRect(page, left, top, page_right, page_bottom, text, HPDF_TALIGN_JUSTIFY, &used_len);
        if(status != HPDF_OK) {
            ErrorHandler::log_to_file("Failed to add text to page");
            break;
        }
        text += used_len;
        size -= used_len;
    }
}