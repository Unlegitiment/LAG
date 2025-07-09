#pragma once
#include <d3d11.h>
#include <vector>
#include <LAG\device\device.h>
class grcStateBlock {
public:
	static void Init() {
		sm_vViewports = new std::vector<D3D11_VIEWPORT>();
		sm_pRasterState = CreateDefaultRasterState();
		assert(sm_pRect != nullptr && __FUNCTION__"Rect is null! Can't create a Viewport off of nothing!");
		sm_vViewports->push_back(CreateViewport(*sm_pRect));
	}
	static void BeginFrame() {
		grcDeviced3d::Get()->context->RSSetState(sm_pRasterState);
		grcDeviced3d::Get()->context->RSSetViewports((UINT)sm_vViewports->size(), sm_vViewports->data()); // yes the cast is intential shut compiler
		grcDeviced3d::Get()->context->IASetPrimitiveTopology(Topology);
	}
	static ID3D11RasterizerState* GetRasterState() { return sm_pRasterState; }
	//Purpose: Assist with clearing or removing state from the D3D11 Immediate Context. 
	static void EndFrame() {
		grcDeviced3d::Get()->context->RSSetState(nullptr);
		grcDeviced3d::Get()->context->RSSetViewports(0, nullptr); // check clear?
		grcDeviced3d::Get()->context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED);

	}
	//One call. Destroy the class. Do not call until end. This frees all resources encompassed by this machine.
	static void DestroyStateBlock() {
		if (sm_pRasterState) sm_pRasterState->Release(); sm_pRasterState = nullptr;
		if (sm_vViewports) sm_vViewports->clear(); delete sm_vViewports; sm_vViewports = nullptr;
	}
	static inline D3D11_PRIMITIVE_TOPOLOGY Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	static inline RECT* sm_pRect = nullptr;
private:
	static inline ID3D11RasterizerState* sm_pRasterState = nullptr;
	static inline std::vector<D3D11_VIEWPORT>* sm_vViewports = nullptr;
	static ID3D11RasterizerState* CreateDefaultRasterState() {
		ID3D11RasterizerState* ret = nullptr;
		D3D11_RASTERIZER_DESC desc = {};
		desc.CullMode = D3D11_CULL_FRONT;
		desc.FrontCounterClockwise = 1;
		desc.FillMode = D3D11_FILL_SOLID;
		HRESULT hr = grcDeviced3d::Get()->outDevice->CreateRasterizerState(&desc, &ret);
		assert(SUCCEEDED(hr)); //cap that mf
		return ret;
	}
	static D3D11_VIEWPORT CreateViewport(const RECT& rect) {
		D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)(rect.right - rect.left), (FLOAT)(rect.bottom - rect.top), 0.0f, 1.0f };
		return viewport;
	}
};