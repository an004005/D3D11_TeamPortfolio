#pragma once

BEGIN(Engine)

class ENGINE_DLL CMathUtils
{
private:
	CMathUtils() = default;

	// 모든 함수는 "SimpleMath.h", "SimpleMath.inl"를 참고하였습니다.

public:
	template<typename T>
	static T Clamp(T Value, T Min, T Max);

	static _bool FloatCmp(_float f1, _float f2, _float fEpsilon = 0.000001f);
	static _float4 Quat2EulerDegree(Quaternion q);
	static _float4 Quat2Euler(Quaternion q);

	static _float Lerp(_float f1, _float f2, _float fS);

	static _float SmoothStep(_float fS);
	static _float Saturate(_float fS);

	// 직선 ab와 점c의 최단거리
	static float DistPointSegment(_float3 a, _float3 b, _float3 c);

	static void BillBoard(_float4x4& WorldMatrix, _float4 vCamPos, _float3 vCamUp, _float3* vCamLook = nullptr);
	static void LookAxisLockBillBoard(_float4x4& WorldMatrix, _float4 vCamPos);
	static void UpAxisLockBillBoard(_float4x4& WorldMatrix, _float4 vCamPos);
	static void RightAxisLockBillBoard(_float4x4& WorldMatrix, _float4 vCamPos);

	static _float RandomFloat(_float fMin = 0.f, _float fMax = 1.f);
	static _float3 RandomUnitVectorInCone_AxisY(_float fConDegree);
	static _float4 RandomPosVectorInCircle_FromZero(_float fRadius);
	static _uint RandomUInt(_uint iMax);

};

template <typename T>
T CMathUtils::Clamp(T Value, T Min, T Max)
{
	if (Value < Min) return Min;
	if (Value > Max) return Max;
	return Value;
}


END
