#pragma once
struct ID3D11VertexShader;
#include "../../lagFramework/renderer/shader.h"
class grcVertexShader : public fwShader {
public:
    grcVertexShader() : fwShader(Vertex) {}
    typedef ID3D11VertexShader* ShaderType; // Crossplatform ability is one HELL OF A BITCH Jesus. Doing it like this though allows me later to use preprocessor if statements to run state checking. Obviously the code for this becomes a mess however. 
    ShaderType GetShader() { return this->Shader; }
private:
    ShaderType Shader;
};