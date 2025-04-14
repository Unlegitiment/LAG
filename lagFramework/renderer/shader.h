#pragma once
class fwShader {
public:
    enum eShaderType {
        Vertex,
        Pixel
    };
    virtual void Bind() = 0;
    eShaderType GetShaderType() { return this->ShaderType; }
protected:
    fwShader(eShaderType type) {
        this->ShaderType = type;
    }
    eShaderType ShaderType;
};