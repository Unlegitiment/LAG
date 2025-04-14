#pragma once
class fwMesh {
public:
    void Bind() {

    }
    // I have to work on this stuff another day because right now this stuff is SORELY lacking in the ability to actually run. 
    //std::vector<uint32_t> GetIndexArray() { return this->m_pGeo[0]. }
    // ffs I hate this. mesh code is so weird smh. 
private:
    Geometry* m_pGeo;
    int numGeo;
};