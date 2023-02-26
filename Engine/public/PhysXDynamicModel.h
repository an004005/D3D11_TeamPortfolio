#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CPhysXDynamicModel : public CComponent
{
protected:
	CPhysXDynamicModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysXDynamicModel(const CPhysXDynamicModel& rhs);
	virtual ~CPhysXDynamicModel() = default;

public:
	virtual HRESULT Initialize_Prototype(const char* pModelFilePath, _float4x4 PivotMatrix);
	virtual HRESULT Initialize(void* pArg);
	virtual void Imgui_RenderProperty() override;

	// 최초 한번만 위치 셋팅하는 용도로 사용할것 그 뒤는 physx에 몸을 맡긴다.
	void SetPxWorldMatrix(const _float4x4& WorldMatrix);
	void Update_AfterPhysX(class CTransform* pTransform);

private:
	static _float4x4 s_DefaultPivot;

private:
	vector<PxMesh> m_PxMeshes;
	physx::PxRigidDynamic* m_pActor = nullptr;

public:
	virtual void Free() override;
	static CPhysXDynamicModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pModelFilePath, _float4x4 PivotMatrix = s_DefaultPivot);
	virtual CComponent* Clone(void* pArg = nullptr);
};

END