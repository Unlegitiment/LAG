#pragma once
#include <Shader\Shadergroup.h>
#include <Buffer\Buffer.h>
#include <Buffer\indexbuffer.h>
#include <Buffer\vertexbuffer.h>
#include <stacktrace>
#include <string>
#include <RenderPass\Renderpass.h>
class grcModel {
public:
	grcModel() {
		m_pShader = new grcShaderGroup(L"W:\\GTAV Scripts\\LAG\\shaders\\VertexShader.hlsl"); // Shader Module which means we have to read from mtl data or from model information Idk
		m_pVertexBuffer = new grcVertexBuffer(fPtr, 32 * sizeof(float), 8 * sizeof(float));
		m_pIndexBuffer = new grcIndexBuffer(mIndex, 6 * sizeof(int));
		//m_pDrawHandler = new CEntityDrawHandler(this);
		if(buffer) m_pShader->AppendShaderConstantBuffer(0, buffer);
	}
	void Draw(float fTime) {
		static int asd = 0;
		Test.colorTint[0] = (sin(fTime) + 1.0f) * 0.5f;
		Test.colorTint[1] = (cos(fTime) + 1.0f) * 0.5f;
		if (asd >= 6000) {
			Test.colorTint[2] = 1.0;
		}
		else {
			Test.colorTint[2] = 0.0f;
		}
		Test.colorTint[3] = 1.0f;

		buffer->Update(&Test, sizeof(TestBuffer)); // fun
		m_pShader->Bind();
		m_pIndexBuffer->Bind();
		m_pVertexBuffer->Bind();
		grcDeviced3d::Get()->context->DrawIndexed(ARRAYSIZE(mIndex), 0, 0);
		asd++;
	}
	~grcModel() {
		delete m_pIndexBuffer;
		m_pIndexBuffer = nullptr;
		delete m_pVertexBuffer;
		m_pVertexBuffer = nullptr;
		delete m_pShader;
		m_pShader = nullptr;
		delete buffer;
		buffer = nullptr;
	}
	//CEntityDrawHandler* GetDrawHandler() { return this->m_pDrawHandler; }
	grcShaderGroup* GetShaderGroup() { return this->m_pShader; }
	grcVertexBuffer* GetVertexBuffer() { return this->m_pVertexBuffer; }
	grcIndexBuffer* GetIndexBuffer() { return this->m_pIndexBuffer; }
private:
	float fPtr[32] = {
		 0.5f,  0.5f, 0.0f,		1.0,0.0,0.0,		1.0,0.0,		// top right
		 0.5f, -0.5f, 0.0f,		0.0,1.0,0.0,		1.0,1.0,		// bottom right
		-0.5f, -0.5f, 0.0f,		0.0,0.0,1.0,		0.0,1.0,		// bottom left
		-0.5f,  0.5f, 0.0f,		1.0,1.0,0.0,		0.0,0.0			// top left 
	};
	int mIndex[6] = {
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
	};
	struct TestBuffer {
		float colorTint[4]; // just testing.
	}Test;
	//CEntityDrawHandler* m_pDrawHandler;
	grcCBuffer* buffer = new grcCBuffer(&Test, sizeof(TestBuffer));
	grcShaderGroup* m_pShader = nullptr;
	grcVertexBuffer* m_pVertexBuffer = nullptr;
	grcIndexBuffer* m_pIndexBuffer = nullptr;
};