#pragma once
#include <DirectXMath.h>
namespace dxm = DirectX;
class Vector3 {
private:
    dxm::XMVECTOR m_Vector = { 0,0,0,1.0f };
public:
    Vector3(dxm::XMVECTOR vector) {
        this->m_Vector = vector;
    }
    Vector3(float x, float y, float z) {
        this->m_Vector = dxm::XMVectorSet(x, y, z, 1.0f);
    }
    Vector3 operator-(const Vector3& x) {
        return dxm::XMVectorSubtract(this->m_Vector, x.m_Vector);
    }
    Vector3 operator+(const Vector3& x) {
        return dxm::XMVectorAdd(this->m_Vector, x.m_Vector);
    }
    Vector3 operator*(const Vector3& x) {
        return dxm::XMVectorMultiply(this->m_Vector, x.m_Vector);
    }
    Vector3 operator/(const Vector3& x) {
        return dxm::XMVectorDivide(this->m_Vector, x.m_Vector);
    }
    float GetX() { return dxm::XMVectorGetX(this->m_Vector); }
    float GetY() { return dxm::XMVectorGetY(this->m_Vector); }
    float GetZ() { return dxm::XMVectorGetZ(this->m_Vector); }
    float GetW() { return dxm::XMVectorGetW(this->m_Vector); }
    dxm::XMVECTOR GetRaw() const { return this->m_Vector; }
    void SetX(float x) { this->m_Vector = dxm::XMVectorSetX(this->m_Vector, x); }
    void SetY(float x) { this->m_Vector = dxm::XMVectorSetY(this->m_Vector, x); }
    void SetZ(float x) { this->m_Vector = dxm::XMVectorSetZ(this->m_Vector, x); }
    void SetW(float x) { this->m_Vector = dxm::XMVectorSetW(this->m_Vector, x); }
    bool IsGreaterThan(const Vector3& x) { return dxm::XMVector3Greater(this->m_Vector, x.m_Vector); }
    bool IsGreaterThanOrEqualTo(const Vector3& x) { return dxm::XMVector3GreaterOrEqual(this->m_Vector, x.m_Vector); }
    bool IsLessThan(const Vector3& x) { return dxm::XMVector3Less(this->m_Vector, x.m_Vector); }
    bool IsLessThanOrEqualTo(const Vector3& x) { return dxm::XMVector3LessOrEqual(this->m_Vector, x.m_Vector); }
    Vector3 Cross(const Vector3& x) { return dxm::XMVector3Cross(this->m_Vector, x.m_Vector); }
    Vector3 Dot(const Vector3& x) { return dxm::XMVector3Dot(this->m_Vector, x.m_Vector); }
    Vector3 Scale(float scale) { return dxm::XMVectorScale(this->m_Vector, scale); }
    Vector3 Normalize() { return dxm::XMVector3Normalize(this->m_Vector); }
};