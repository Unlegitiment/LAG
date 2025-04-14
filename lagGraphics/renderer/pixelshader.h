#pragma once
struct ID3D11PixelShader;
#include "../../lagFramework/renderer/shader.h"
class grcFragShader : public fwShader {
public:
    typedef ID3D11PixelShader* ShaderType;
    ShaderType GetShader() { return this->Shader; }
    void Bind() {

    }
private:
    ShaderType Shader;
};