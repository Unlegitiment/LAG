#pragma once

#include "LIT\types\integrals.h"
#include <vector>

class CGameModel {
public:
private:

};
template<typename ImporterType, typename SceneType, typename MeshType> struct ImporterParams {
	using ImporterBase = ImporterType;
	using ImporterScene = SceneType;
	using Mesh = MeshType;
};
using NullParams = ImporterParams<void, void, void>;
template<typename ImpParams> // This is required for the template declarations
class CGameModelLoader {
public:
private:

};
class CVector3 {
public:
	float x, y, z; // sample.
};
class CVector2 {
public:
	float x, y;
};
struct VertexSpecifier {
	VertexSpecifier(CVector3 v1, CVector2 v2) : vPos(v1), vUVCoord(v2) {};
	CVector3 vPos;
	CVector2 vUVCoord;
};
class grcGeometry { // geometry
public:
	grcGeometry(std::vector<VertexSpecifier> vertexpos, std::vector<lit::u32> indices) {
		this->m_Geometry = vertexpos;
		this->m_Indices = indices;
		//this->m_UV = UV;
	}
	std::vector<VertexSpecifier>& GetGeometry() { return m_Geometry; }
	std::vector<lit::u32>& GetIndices() { return m_Indices; }
	std::vector<CVector2>& GetUV() { return m_UV; }
private:
	std::vector<VertexSpecifier> m_Geometry;
	std::vector<lit::u32> m_Indices;
	std::vector<CVector2> m_UV;
};
#define USE_ASSIMP // we are just gonna use default as a testing ground but its extensible. 
#ifdef USE_ASSIMP
#include <iostream>
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"
#include <numeric>
using AssimpImport = ImporterParams<Assimp::Importer, aiScene, aiMesh>;
template<>
class CGameModelLoader<AssimpImport> {
private:
	using Importer = typename AssimpImport::ImporterBase;
	using Scene = typename AssimpImport::ImporterScene;
	using Mesh = typename AssimpImport::Mesh;
public: 
	CGameModelLoader(const char* fileName, lit::u32 flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs) { // @Todo: Add Baggie of parameters here. For multi-compilations or just specify it inside of another thing. Eitherway need someway to do funnies. 
		m_FileName = fileName;  // might not be safe. 
		m_iFlags = flags; 
	}
	void FillImporter() {
		m_pScene = this->m_Importer.ReadFile(m_FileName, m_iFlags);
		if (!m_pScene) {
			throw std::exception("Model has failed to import.");
		}
	}
	const Scene* GetScene() const { return m_pScene; }
	const Scene** GetScenePtr() { return &m_pScene; }
	//Caller must delete geometry when it is necessary. 
	std::vector<grcGeometry*> CreateGeometry() { // technically a model. 
		std::vector<grcGeometry*> SceneGeometry;
		Recursive(this->m_pScene->mRootNode, SceneGeometry);
		return SceneGeometry;
	}
private:
	void Recursive(aiNode* pNode, std::vector<grcGeometry*>& SceneGeometry){
		for (unsigned int i = 0; i < pNode->mNumMeshes; i++) {
			unsigned int meshIndex = pNode->mMeshes[i];
			aiMesh* mesh = m_pScene->mMeshes[meshIndex];
			if (mesh->HasPositions()) {
				SceneGeometry.push_back(LoadOneModel(mesh));
			}
		}
		for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
			Recursive(pNode->mChildren[i], SceneGeometry);
		}
	}
	std::vector<lit::u32> GetIndices(Mesh* mesh) const {
		if (!mesh || !mesh->HasFaces()) {
			throw std::runtime_error("Passed mesh is either null or doesn't have faces.");
		}
		if (!(m_iFlags & aiProcess_Triangulate)) {
			throw std::runtime_error("Failed to give triangulate flag to model initialization.");
		}
		std::vector<lit::u32> Indices;
		Indices.reserve(mesh->mNumFaces * 3);
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			const aiFace& curFace = mesh->mFaces[i];
			if (curFace.mNumIndices != 3) {
				printf("Warning: Non-tri face found: %d indices\n", curFace.mNumIndices);
				continue;
			}
			for (unsigned int j = 0; j < 3; j++) {
				Indices.push_back(curFace.mIndices[j]);
			}
		}
		return Indices;
	}
	std::vector<CVector2> GetUv(Mesh* mesh) const {
		if (!mesh || !mesh->HasTextureCoords(0)) return {};

		std::vector<CVector2> vec;
		vec.reserve(mesh->mNumVertices);

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D uv = mesh->mTextureCoords[0][i];
			vec.push_back({ uv.x, uv.y });
		}

		return vec;
	}
#include <assimp/material.h>
	std::vector<VertexSpecifier> GetVertexData(Mesh* mesh) const {
		std::vector<VertexSpecifier> vertices;
		if (!mesh || !mesh->HasPositions() || !mesh->HasTextureCoords(0)) return vertices;
		mesh->mMaterialIndex;
		aiMaterial* mat = m_pScene->mMaterials[mesh->mMaterialIndex];
		aiString str;
		if (mat->GetTexture(aiTextureType_AMBIENT, 0, &str) == aiReturn_SUCCESS) {

		}
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			const aiFace& face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j) {
				unsigned int index = face.mIndices[j];
				const aiVector3D& pos = mesh->mVertices[index];
				const aiVector3D& uv = mesh->mTextureCoords[0][index];

				vertices.push_back({
					CVector3{ pos.x, pos.y, pos.z },
					CVector2{ uv.x, uv.y }
					});
			}
		}
		return vertices;
	}
	std::vector<const char*> GetTextures() {
	
	}
	grcGeometry* LoadOneModel(Mesh* mesh) const {
		std::vector<VertexSpecifier> verts = GetVertexData(mesh);
		std::vector<lit::u32> indices(verts.size());
		std::iota(indices.begin(), indices.end(), 0);

		return new grcGeometry(std::move(verts), std::move(indices));
		//return new grcGeometry(GetVertexData(mesh), this->GetIndices(mesh)); // larger models are gonna be really annoying, but this should be done pretty much last or when the model actually NEEDS to be acquired from the game. 
		// I kinda need a system around this that stores the models and allows me to fetch them via like their name or something like that? 
	}
	CVector3 ConvertModelVectorToCommon(aiVector3D& vec) const {
		CVector3 ret{};
		ret.x = vec.x;
		ret.y = vec.y;
		ret.z = vec.z;
		return ret;
	}
private: 
	lit::u32 m_iFlags = 0;
	const char* m_FileName = nullptr;
	Importer m_Importer; // 
	const Scene* m_pScene = nullptr;
};
#endif // USE_ASSIMP
using ModelImporter = CGameModelLoader<
#ifdef USE_ASSIMP 
	AssimpImport
#else
	NullParams
#endif
>;