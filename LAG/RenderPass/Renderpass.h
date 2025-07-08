#pragma once
#include <unordered_map>
#include <LAG\Shader\Shadergroup.h>
#include <LAG\model\grcmodel.h>
#define RENDER_PASS_MAX_BUFFERS 5
//class fwDrawCommand {
//public:
//	virtual void Execute() = 0;
//private:
//
//};
//class RenderQueue {
//public:
//	virtual void SubmitToRenderpass(fwDrawCommand*) = 0;
//protected:
//	std::vector<fwDrawCommand*> DrawCommands;
//};
//class CEntityRenderQueue : public RenderQueue {
//public:
//	void SubmitToRenderpass(fwDrawCommand* cmd) override
//	{
//		DrawCommands.push_back(cmd);
//	}
//	void Execute() {
//		renderpass.Execute();
//		DrawCommands.clear(); 
//	}
//private:
//	RenderPass renderpass = RenderPass(&DrawCommands);
//};
//class RenderPass{
//public:
//	RenderPass(std::vector<fwDrawCommand*>* drawCommands) {
//
//	}
//	void AppendPassShader(int regist, grcCBuffer* buffer) {
//		if (m_mBuffers.size() > RENDER_PASS_MAX_BUFFERS) {
//			throw std::runtime_error(__FUNCTION__" Renderpass buffer conflicts with Object Buffer. Make your Renderpass buffers store more information ");
//		}
//		m_mBuffers.insert({regist, buffer});
//	}
//	void Execute() {
//		assert(m_vDrawCommands && "Commands don't exist. There is a crucial error in the runtime. We must exit");
//		
//		for (auto* command : *m_vDrawCommands) {
//			command->Execute();
//		}
//	}
//private:
//	std::vector<fwDrawCommand*>* m_vDrawCommands;
//	std::unordered_map<int, grcCBuffer*> m_mBuffers;
//};
//
//class CEntityDrawCommand : public fwDrawCommand {
//public:
//	CEntityDrawCommand(grcModel* model) {
//
//	}
//	void Execute() {
//		
//	}
//private:
//	grcModel* model; 
//};
//class CEntityDrawHandler {
//public:
//	CEntityDrawHandler(grcModel* model) {
//		this->m_Model = model;
//		this->m_pDrawCommand = new CEntityDrawCommand(this->m_Model);
//	}
//	grcModel* GetModel() { return m_Model; }
//	void Render() {
//		// :hmm:
//			
//	}
//private:
//	grcModel* m_Model = nullptr;
//	CEntityDrawCommand* m_pDrawCommand = nullptr;
//};
//class fwEntity {
//public:
//private:
//	CEntityDrawHandler* m_pDrawHandler = nullptr;
//};