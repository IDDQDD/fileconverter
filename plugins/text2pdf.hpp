#pragma once
#include <optional>
#include <vector>
#include <hpdf.h>
#include "IConverterFactory.hpp"

class Text2Pdf : public IConverterFactory {
    std::vector<HPDF_BYTE> pdf_data;
    public:
        Text2Pdf() = default;
        ~Text2Pdf() override = default;

        std::string support_mime_type() const override;
        std::string target_format() const override;
        std::optional<ConvertResult> convert(const void *data, std::size_t size) override;
        private:
            void AddTextWithMargins(HPDF_Doc pdf, const char *data,std::size_t size, HPDF_Font font, std::size_t font_size = 14,
                                    float left = 60, float right = 50, float top = 60, float bottom = 60);
};