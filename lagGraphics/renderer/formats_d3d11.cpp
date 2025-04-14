#include "formats_d3d11.h"
#include <d3d11.h>
grcD3D11FormatMappings* grcD3D11FormatMappings::sm_pFormatMapperDXGI = new grcD3D11FormatMappings();

void grcD3D11FormatMappings::Init()
{
    this->m_FormatMap.insert({ fwRenderContext::eFormat::FMT_R32_UINT, DXGI_FORMAT::DXGI_FORMAT_R32_UINT }); // this is a bunch of shit. regardless of your opinion :L.

}

DXGI_FORMAT grcD3D11FormatMappings::TranslateFormat(fwRenderContext::eFormat format)
{
    return this->m_FormatMap.at(format);
}
