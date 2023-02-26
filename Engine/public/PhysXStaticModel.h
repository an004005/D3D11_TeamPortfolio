#pragma once
#include "Component.h"

BEGIN(Engine)

struct PxMesh
{
	physx::PxTriangleMesh* pPxMesh = nullptr;
	physx::PxTriangleMeshGeometry tMeshGeom;
	physx::PxShape* pShape = nullptr;
};

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

END