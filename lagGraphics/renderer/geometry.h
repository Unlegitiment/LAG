#pragma once
#include <vector>
#include <assimp\scene.h>
//@TODO: Add the functionality to this class to allow for the dynamic vertex stuff to occur. 
class grcVertexBuffer;
class grcIndexBuffer;
/// <summary>
/// Our goal here is to represent a Single Node in a scene graph that builds to mesh.
/// --Problem is that I need someway to load scene graphs. Which means I need to basically build it here. I need a MeshLib somewhere Assimp? 
/// </summary>
class grcGeometry {
public:
	grcVertexBuffer* GetVertexBuffer() { return this->m_pVertexBuffer;  }
	grcVertexBuffer** GetAddressOfVertexBuffer() { return &this->m_pVertexBuffer; }
	grcIndexBuffer* GetIndexBuffer() { return this->m_pIndexBuffer; }
	grcIndexBuffer** GetAddressOfIndexBuffer() { return &this->m_pIndexBuffer; }
	// This is where I need an aiNode* so that I can get access to geo specific data :| Otherwise you have to parse the model anyways which I REALLY do not want to do right now.
	void Init(aiNode* pNode) {

	}
	void Destroy() {

	}
private:
	grcVertexBuffer* m_pVertexBuffer;
	grcIndexBuffer* m_pIndexBuffer;
};