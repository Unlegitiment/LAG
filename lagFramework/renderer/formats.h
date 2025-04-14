#pragma once
#include <map>
#include "rendercontext.h"
template<typename ConversionType>
class fwFormatMappings {
public:
    virtual ConversionType TranslateFormat(fwRenderContext::eFormat format) = 0;
protected:
    std::map<fwRenderContext::eFormat, ConversionType> m_FormatMap;
};