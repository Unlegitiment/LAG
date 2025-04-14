#pragma once
//@TODO: Move all function definitions to private .cpp file!!!
#include "../../lagFramework/renderer/rendercontext.h"
#include "../../lagFramework/renderer/shadergroup.h"
#include "mesh.h"
class grcDrawable {
public:
    grcDrawable(fwMesh* msh, fwShaderGroup* pShader) {
        this->m_pMesh = msh;
        this->m_pShaders = pShader; // fuckshit
    }
    void SetShader(fwShaderGroup* pShader) {
        this->m_pShaders = pShader; // i don't care if pShader is null :)
    }
    fwMesh& GetMesh() { return *m_pMesh; }
    fwShaderGroup& GetShaderGroup() { return *m_pShaders; }
    void SetupDraw(fwRenderContext* renderContext) {

    }
private:
    fwRenderContext* m_pRenderContext;
    fwMesh* m_pMesh;
    fwShaderGroup* m_pShaders;
};