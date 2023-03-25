#pragma once
#include "Component.h"

/* ��ü�� ���� �����̽� �󿡼��� ���¸� �������ش�.(m_WorldMatrix) */
/* ���º�ȯ�� ���� �������̽��� �������ش�.(Go_Straight, Turn, Chase, LookAt) */

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	/* STATE_TRANSLATION : (���� �Ǵ� �θ�) �������κ��� �󸶳� �������־�. */
	/* Position : �������� ������������� ��ġ. */
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_TRANSLATION, STATE_END };

	typedef struct tagTransformDesc
	{
		/* �ʴ� ���������ϴ� �ӵ�. */
		float		fSpeedPerSec;

		/* �ʴ� ȸ���ؾ��ϴ� �ӵ�. */
		float		fRotationPerSec;
	}TRANSFORMDESC;

protected:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_matrix Get_WorldMatrix_Inverse();
	_matrix Get_WorldMatrix();
	_float4x4 Get_WorldMatrix_f4x4();
	void Set_WorldMatrix(const _float4x4& WorldMatrix) { m_WorldMatrix = WorldMatrix; };
	void Set_WorldMatrix(_fmatrix WorldMatrix) { XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix); }
	_vector Get_State(STATE eState) const {
		return XMLoadFloat4x4(&m_WorldMatrix).r[eState];
	}
	_float GetYaw_Radian();
	_float GetPitch_Radian();

	_float3 Get_Scaled() const {
		return _float3(XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))), 
			XMVectorGetX(XMVector3Length(Get_State(STATE_UP))), 
			XMVectorGetX(XMVector3Length(Get_State(STATE_LOOK))));
	}

	_float GetRotPerSec() const { return m_TransformDesc.fRotationPerSec; }

	void Set_State(STATE eState, _fvector vState) {
		_float4		vTmp;
		XMStoreFloat4(&vTmp, vState);
		memcpy(&m_WorldMatrix.m[eState][0], &vTmp, sizeof vTmp);			
	}

	void Set_Scaled(STATE eState, _float fScale); /* fScale������ ���̸� �����Ѵ�. */
	void Set_Scaled(_float3 vScale); /* fScale������ ���̸� �����Ѵ�. */
	void Scaling(STATE eState, _float fScale); /* fScale����� �ø���. */

	void SetTransformDesc(TRANSFORMDESC Desc) { m_TransformDesc = Desc; }
	void SetSpeed(_float fSpeedPerSec) { m_TransformDesc.fSpeedPerSec = fSpeedPerSec; }
	void SetRotPerSec(_float fRotationPerSec) { m_TransformDesc.fRotationPerSec = fRotationPerSec; }

	void SetPitchLock_Deg(_float fMaxPitchDeg) { m_bPitchLock = true; m_fMaxPitchRad = fabs(XMConvertToRadians(fMaxPitchDeg)); }
public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Imgui_RenderProperty() override;
	virtual void SaveToJson(OUT Json& json) override;
	virtual void LoadFromJson(const Json& json) override;

	void Go_Straight(_double TimeDelta);
	void Go_Backward(_double TimeDelta);
	void Go_Left(_double TimeDelta);
	void Go_Right(_double TimeDelta);
	void Move(_double TimeDelta, _float3 vDir);
	void MoveVelocity(_double TimeDelta, _float3 vVelocity);
	void MoveVelocity(_double TimeDelta, _float4 vVelocity);
	_vector MoveVelocity_Get(_double TimeDelta, _float3 vVelocity);
	void MoveVelocity_Navi(_double TimeDelta, _float3 vVelocity, class CNavigation* pNavi, _int& iCellIdx);
	void LocalMove(_float3 vDir, _float fRange = 1.f);
	void AddQuaternion(_vector Quaternion);

	/* ��ü�� Ư�� ���� �Է¹��� ������ ��ȯ�ϰ� �����Ͽ� �� ��ȯ */
	void SetAxis(STATE eState, _fvector vAxis);
	void RightAt(_fvector vAxis);
	void RightAt_Smooth(_fvector vAxis, _double TimeDelta);

	// Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta);
	void Turn(_fvector vAxis, _double TimeDelta); /* Dynamic */
	void Turn_Fixed(_fvector vAxis, _float fRadian); /* Dynamic */
	void TurnPitch(_double TimeDelta);
	void Rotation(_fvector vAxis, _float fRadian); /* Static */
	void RemoveRotation();
	void TurnByMatrix(_fmatrix turnMatrix);
	/* �Ĵٺ���. */
	/*void LookAt(const CTransform* pTarget);*/
	void LookAt(_fvector vTargetPos);
	void LookAt_NonY(_fvector vTargetPos);
	void LookAt_Smooth(_fvector vTargetPos, _double TimeDelta);
	_float Get_RemainYawToLookAt(_fvector vTargetPos);
	_bool LookAt_Lerp(_fvector vSourLook, _fvector vTargetPos, _float fLerp);
	void LookAt_Lerp_Test(_fvector vTargetPos, _float fLerp);

	/* �����Ѵ� .*/
	void Chase(_fvector vTargetPos, _double TimeDelta, _float fLimit = 0.1f);
	void BillBoard(_float4 vCamPos, _float3 vCamUp, _float3* vCamLook = nullptr);
	void ConstrainBillBoard(_float4 vCamPos, _float4 vRotateAxis, _float4* pCamLook = nullptr, _float4* pLook = nullptr);
	void LookAxisLockBillBoard(_float4 vCamPos);
	void UpAxisLockBillBoard(_float4 vCamPos);
	void RightAxisLockBillBoard(_float4 vCamPos);

	void CopyState(STATE eState, CTransform* pFrom);
	void CopyWorld(CTransform* pFrom);

	/* �����Ѵ� */
	//ƽ���� �̵��� �Ÿ� ��ȯ
	_float RushToTarget(_fvector vTargetPos, _double TimeDelta);
public:
	HRESULT Bind_ShaderResource(class CShader* pShaderCom, const char* pConstantName);

public:
	static void ModifyTransformJson(Json& json, _float4x4 WorldMatrix);
	static void MoveTransformJson(Json& jsonDest, const Json& jsonSour);

private:	
	_float4x4				m_WorldMatrix;
	TRANSFORMDESC			m_TransformDesc;

	_bool m_bPitchLock = false;
	_float m_fMaxPitchRad = 0.f;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};


END