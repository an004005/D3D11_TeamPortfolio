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
	void Update_AfterPhysX(class CTransform* pTransform);

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

END