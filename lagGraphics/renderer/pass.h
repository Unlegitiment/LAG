#pragma once
#include <vector>
#include "drawable.h"
class grcPass {
public:
    grcPass(std::vector<grcDrawable*>& ppDrawables) {
        this->m_ppDrawables = ppDrawables;
    }
    void Render() {
        for (auto* draw : m_ppDrawables) {
            draw->SetupDraw();
            m_pRenderContext->DrawIndexed();
        }
    }
private:
    fwRenderContext* m_pRenderContext;
    std::vector<grcDrawable*> m_ppDrawables;
};