#pragma once
#include <DirectXMath.h>
class CCamera {
public:
	DirectX::XMVECTOR POSITION = DirectX::XMVectorSet(0.0, 0.0, 0.0, 1.0f);
	DirectX::XMVECTOR FRONT = DirectX::XMVectorSet(0.0, 0.0, -1.0f, 0.0f);
	DirectX::XMVECTOR UP = DirectX::XMVectorSet(0.0, 1.0, 0.0, 0.0f);
	DirectX::XMVECTOR RIGHT = DirectX::XMVectorSet(0.0,0.0,0.0,0.0);
	DirectX::XMVECTOR WORLDUP = DirectX::XMVectorSet(0.0, 1.0, 0.0, 0.0f);
	float Yaw = -90.0;
	float Pitch = 0.0f;
	float Zoom = 45.f;
	CCamera() {
		UpdateCamVec();
	}
	void Update() {
		UpdateCamVec();
	}
	DirectX::XMMATRIX GetViewMatrix() {
		DirectX::XMVECTOR vec = DirectX::XMVectorAdd(POSITION, FRONT);
		return DirectX::XMMatrixLookAtLH(POSITION, vec, UP);
	}
private:
	void UpdateCamVec() {
		DirectX::XMFLOAT3 front;
		front.x = cos(DirectX::XMConvertToRadians(Yaw)) * cos(DirectX::XMConvertToRadians(Pitch));
		front.y = sin(DirectX::XMConvertToRadians(Pitch));
		front.z = sin(DirectX::XMConvertToRadians(Yaw)) * cos(DirectX::XMConvertToRadians(Pitch));
		FRONT = DirectX::XMLoadFloat3(&front);
		FRONT = DirectX::XMVector3Normalize(FRONT);
		RIGHT = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(FRONT, WORLDUP));
		UP = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(RIGHT, FRONT));
	}
};