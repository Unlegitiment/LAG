#pragma once
#include "LAGFramework\renderer\formats.h"
#include "LAGFramework\renderer\rendercontext.h"
enum DXGI_FORMAT;
class grcD3D11FormatMappings : public fwFormatMappings<DXGI_FORMAT> {
public:
    void Init();
    DXGI_FORMAT TranslateFormat(fwRenderContext::eFormat format);
    static grcD3D11FormatMappings& GetInstance() {
        if (sm_pFormatMapperDXGI != nullptr) {
            return *sm_pFormatMapperDXGI;
        }
        else {
            throw std::exception("[Fatal]: Singleton failed D3D11FormatMappings");
        }
    }
private:
    grcD3D11FormatMappings() { this->Init(); }
    static grcD3D11FormatMappings* sm_pFormatMapperDXGI;
};