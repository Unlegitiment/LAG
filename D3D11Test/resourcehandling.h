#pragma once
struct ID3D11Resource;
typedef long long int64_t;
#include <map>
class grcResource {
public:
    friend class grcResourceMgr;
    enum eResourceType {
        TEXTURE2D,
        BUFFER,
    };
    grcResource(ID3D11Resource* res, eResourceType resType, int64_t hash) {
        this->m_Hash = hash;
        this->m_pResource = res;
        this->m_iType = resType;
    }
    ID3D11Resource* GetResource() { return this->m_pResource; }
    eResourceType GetResourceType() { return this->m_iType; }
    int64_t GetHash() { return this->m_Hash; }
private:
    ID3D11Resource* m_pResource;
    eResourceType m_iType;
    int64_t m_Hash;
};
class grcResourceMgr {
public:
    static void CreateInstance();
    static grcResourceMgr* GetInstance();
    grcResource* Get(int64_t hash);
    bool Add(grcResource* res);
    //eventually add logic for a santity check meaning that we don't cache resources we see infrequently. 
    ~grcResourceMgr() { delete m_ResourceManager; }
private:
    static grcResourceMgr* m_ResourceManager;
    std::map<int64_t, grcResource*> m_Resources;
};
