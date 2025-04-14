#pragma once
#include "shader.h"
class fwShaderGroup {
public:
    fwShaderGroup() {}
    void SetShader(fwShader* pShader) {
        switch (pShader->GetShaderType()) {
        case fwShader::Vertex:
            this->m_pVertexShader = pShader;
            break;
        case fwShader::Pixel:
            this->m_pPixelShader = pShader;
            break;
        }
    }
    fwShader* GetVertexShader() { return this->m_pVertexShader; }
    fwShader* GetPixelShader() { return this->m_pPixelShader; }
private:
    fwShader* m_pVertexShader = nullptr;
    fwShader* m_pPixelShader = nullptr;
};