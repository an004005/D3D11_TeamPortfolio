#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CPhysXStaticModel : public CComponent
{
protected:
	CPhysXStaticModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysXStaticModel(const CPhysXStaticModel& rhs);
	virtual ~CPhysXStaticModel() = default;

public:
	virtual HRESULT Initialize_Prototype(const char* pModelFilePath, _float4x4 PivotMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual void Imgui_RenderProperty() override;

	// 최초 한번만 위치 셋팅하는 용도로 사용할것
	void SetPxWorldMatrix(const _float4x4& WorldMatrix);

private:
	static _float4x4 s_DefaultPivot;

private:
	vector<PxMesh> m_PxMeshes;
	physx::PxRigidStatic* m_pActor = nullptr;

public:
	virtual void Free() override;
	static CPhysXStaticModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pModelFilePath, _float4x4 PivotMatrix = s_DefaultPivot);
	virtual CComponent* Clone(void* pArg = nullptr);
};

class ENGINE_DLL CPhysXStaticMesh : public CComponent
{
protected:
	CPhysXStaticMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysXStaticMesh(const CPhysXStaticMesh& rhs);
	virtual ~CPhysXStaticMesh() = default;

public:
	virtual HRESULT Initialize_Prototype(const vector<VTXNORTEX>& VtxData, const vector<FACEINDICES32>& IdxData, _float4x4 WorldMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual void Imgui_RenderProperty() override;

	// 최초 한번만 위치 셋팅하는 용도로 사용할것
	void SetPxWorldMatrix(const _float4x4& WorldMatrix);
	void Activate(_bool bActive);
	_bool IsActive();

private:
	PxMesh m_PxMesh;
	physx::PxRigidStatic* m_pActor = nullptr;

public:
	virtual void Free() override;
	static CPhysXStaticMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const vector<VTXNORTEX>& VtxData, const vector<FACEINDICES32>& IdxData, _float4x4 WorldMatrix);
	virtual CComponent* Clone(void* pArg = nullptr);
};

END