#pragma once
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11DeviceContext; 
#include "vertexshader.h"
#include "pixelshader.h"
#include "../../lagFramework/renderer/rendercontext.h"

//@TODO: Move All code into .cpp file to avoid header conflicts.  
class grcD3D11RenderContext : public fwRenderContext {
public:
    grcD3D11RenderContext() {}
    void SetInputAssembly(ID3D11InputLayout* pLayout) {
        this->GetBasisContext()->IASetInputLayout(pLayout);
    }
    void SetVertexBuffers(unsigned int StartSlot, unsigned int NumBuffers, void* const* ppVertexBuffers, const unsigned int* pStrides, const unsigned int* pOffsets) {
        this->GetBasisContext()->IASetVertexBuffers(StartSlot, NumBuffers, (ID3D11Buffer* const*)ppVertexBuffers, pStrides, pOffsets); // nice beautiful dirty cast in the middle of the argument :|
    }
    void SetIndexBuffer(void* pIndexBuffer, eFormat Format, unsigned int offset) {
        this->GetBasisContext()->IASetIndexBuffer((ID3D11Buffer*)pIndexBuffer, grcD3D11FormatMappings::GetInstance().TranslateFormat(Format), offset);
    }
    void DrawIndexed(unsigned int indexcount, unsigned int startindexloc, int basevertexlocation) override {
        this->GetBasisContext()->DrawIndexed(indexcount, startindexloc, basevertexlocation);
    }
    void SetVertexShader(void* pRawShader, fwShader* pShader) { // we can reasonably infer that since this is platform specific I can cast this to a fwCommonShader_D3D which allows me to then infer the type according to the data preallocated in fwShader. 
        this->m_CurrentVertexShader = (grcVertexShader*)pShader;
        m_pDeviceContext->VSSetShader((ID3D11VertexShader*)pRawShader, nullptr, 0); // ugh I hate this type of shit :(. 
    }
    void SetFragmentShader(void* pRawShader, fwShader* pShader) {
        this->m_CurrentPixelShader = (grcFragShader*)pShader;
        m_pDeviceContext->PSSetShader((ID3D11PixelShader*)pRawShader, nullptr, 0);
    }
    ID3D11DeviceContext* GetBasisContext() { return this->m_pDeviceContext; }
private:
    ID3D11DeviceContext* m_pDeviceContext;
};