#include "resourcehandling.h"
grcResourceMgr* grcResourceMgr::m_ResourceManager = nullptr;
void grcResourceMgr::CreateInstance() {
    auto* temp = new grcResourceMgr();
    if (!temp) {
        return;
    }
    m_ResourceManager = temp;
}

grcResourceMgr* grcResourceMgr::GetInstance() {
    return m_ResourceManager;
}

grcResource* grcResourceMgr::Get(int64_t hash) {
    auto res = m_Resources.find(hash);
    if (res != m_Resources.end()) {
        return res->second;
    }
    return nullptr;
}

bool grcResourceMgr::Add(grcResource* res) {
    this->m_Resources.insert({ res->m_Hash, res });
    return true;
}
