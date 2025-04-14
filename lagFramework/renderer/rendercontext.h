#pragma once
struct ID3D11InputLayout;
class fwShader;
class fwRenderContext {
public:
    enum eFormat { // @TODO Add more data types as necessary. It's gonna be a pain at first cause its basically data transfusion. Maybe do a bit of automation?  
        FMT_R32_UINT,
    };
    virtual void SetInputAssembly(ID3D11InputLayout* pLayout) = 0; // @TODO Remove ID3D11InputLayout base implementation. 
    virtual void SetVertexBuffers(unsigned int StartSlot, unsigned int NumBuffers, void** ppVertexBuffers, const unsigned int* pStrides, const unsigned int* pOffsets) = 0;
    virtual void SetIndexBuffer(void* pIndexBuffer, eFormat Format, unsigned int offset) = 0; // abstraction yay!
    virtual void DrawIndexed(unsigned int indexcount, unsigned int startindexloc, int basevertexlocation) = 0;
    virtual void SetVertexShader(void* pRawShader, fwShader* pShader) = 0;
    virtual void SetFragmentShader(void* pRawShader, fwShader* pShader) = 0;
protected:
    fwShader* m_CurrentVertexShader;
    fwShader* m_CurrentPixelShader;
};