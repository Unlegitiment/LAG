#pragma once
#include <LAG\Shader\Shadergroup.h>
#include <LAG\Buffer\Buffer.h>
#include <LAG\Buffer\indexbuffer.h>
#include <LAG\Buffer\vertexbuffer.h>
#include <string>
#include <LAG\RenderPass\Renderpass.h>
#include <DirectXMath.h>
#include <LAG\Cam\Camera.h>
class CCameraMgr {
public:
	static void Init() {
		CCameraMgr::sm_pInstance = new CCameraMgr();
	}
	int PushNewCamera(CCamera* camera) {
		this->m_Camera.push_back(camera);
		return(int) this->m_Camera.size() - 1; // index
	}
	void RemoveCameraAtIndex(int index) {
		assert(index <= this->m_Camera.size() && __FUNCTION__"Passed argument larger than expected entry");
		m_Camera.erase(m_Camera.begin() + index);
	}
	void ActivateCamera(int index) {
		assert(index <= this->m_Camera.size() && __FUNCTION__"Passed argument larger than expected entry");
		m_CurrentCamera = m_Camera[index];
	}
	static void Destroy() {
		if (sm_pInstance) delete sm_pInstance; sm_pInstance = nullptr;
	}
	static CCameraMgr& Get() {
		assert(sm_pInstance!=nullptr && __FUNCTION__"sm_pInstance nullptr");
		return *sm_pInstance;
	}
	static CCamera& GetCurrentCamera() {
		CCamera* camera = Get().m_CurrentCamera;
		assert(camera != nullptr && "m_CurrentCamera is not set! Default initialize one in the Scene!");
		return *camera;
	}
private:
	static inline CCameraMgr* sm_pInstance = nullptr;
	std::vector<CCamera*> m_Camera;
	CCamera* m_CurrentCamera = nullptr;
};
//class grcModel {
//	grcGeometry* m_pGeometry = nullptr;
//public:
//	CCamera* camera = new CCamera();
//	grcModel(grcGeometry* geometry) {
//		this->m_pGeometry = geometry;
//		m_pShader = new grcShaderGroup(L"W:\\GTAV Scripts\\LAG\\shaders\\VertexShader.hlsl"); // Shader Module which means we have to read from mtl data or from model information Idk
//		m_pVertexBuffer = new grcVertexBuffer(geometry->GetGeometry().data(), (UINT)(geometry->GetGeometry().size() * sizeof(VertexSpecifier)), 5 * sizeof(float));
//		m_pIndexBuffer = new grcIndexBuffer(geometry->GetIndices().data(), (UINT)geometry->GetIndices().size() * sizeof(int));
//		if(buffer) m_pShader->AppendShaderConstantBuffer(0, buffer);
//		CCameraMgr::Init();
//		int retVal = CCameraMgr::Get().PushNewCamera(camera);
//		CCameraMgr::Get().ActivateCamera(retVal); // woohoo.
//	}
////#include "thirdparty\imgui.h"
//	void Draw(float x, float y, float z) {
//		this->camera->Update(); // move to scene have the current scene own the camera lend to Renderpass or just hand a smaller object. /shrug
//		Test.model = DirectX::XMMatrixRotationZ(0.0f) * DirectX::XMMatrixScaling(1, 1, 1) * DirectX::XMMatrixTranslation(x, y, z);
//		Test.view = camera->GetViewMatrix();
//		Test.projection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(camera->Zoom), 1920.f / 1080.f, 0.5f, 10000.0f);
//		Test.Transform = DirectX::XMMatrixTranspose(Test.model * Test.view * Test.projection);
//		buffer->Update(&Test, sizeof(Constants)); // fun
//		m_pShader->Bind();
//		m_pIndexBuffer->Bind();
//		m_pVertexBuffer->Bind();
//		grcDeviced3d::Get()->context->DrawIndexed((UINT)m_pGeometry->GetIndices().size(), 0, 0); //wdym only 36? theres a lot more vertices. (later) can I read? am I just an idiot? 
//	}
//	~grcModel() {
//		delete m_pIndexBuffer;
//		m_pIndexBuffer = nullptr;
//		delete m_pVertexBuffer;
//		m_pVertexBuffer = nullptr;
//		delete m_pShader;
//		m_pShader = nullptr;
//		delete buffer;
//		buffer = nullptr;
//		//move to Scene.
//		delete camera; camera = nullptr;
//		CCameraMgr::Destroy();
//	}
//	//CEntityDrawHandler* GetDrawHandler() { return this->m_pDrawHandler; }
//	grcShaderGroup* GetShaderGroup() { return this->m_pShader; }
//	grcVertexBuffer* GetVertexBuffer() { return this->m_pVertexBuffer; }
//	grcIndexBuffer* GetIndexBuffer() { return this->m_pIndexBuffer; }
//private:
//	struct Constants {
//		DirectX::XMMATRIX model;
//		DirectX::XMMATRIX view;
//		DirectX::XMMATRIX projection;
//		DirectX::XMMATRIX Transform;
//	}Test;
//	//CEntityDrawHandler* m_pDrawHandler;
//	grcCBuffer* buffer = new grcCBuffer(&Test, sizeof(Constants));
//	grcShaderGroup* m_pShader = nullptr;
//	grcVertexBuffer* m_pVertexBuffer = nullptr;
//	grcIndexBuffer* m_pIndexBuffer = nullptr;
//};