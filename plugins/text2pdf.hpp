#pragma once
#include "IConverterFactory.hpp"

class Text2Pdf : public IConverterFactory {
    const void *data;
    public:
        Text2Pdf() = default;
        ~Text2Pdf() override = default;

        std::string support_mime_type() const override;
        std::string target_format() const override;
        const void* convert(const void *data, std::size_t size) override;
        private:
            void AddTextWithMargins(HPDF_Doc pdf, const char *data,std::size_t size, HPDF_Font font, size_t font_size = 14,
                                    float left = 60, float right = 50, float top = 60, float bottom = 60);
};