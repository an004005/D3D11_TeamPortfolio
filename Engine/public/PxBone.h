#pragma once
#include "Bone.h"

BEGIN(Engine)

class CPxBone : public CBone
{
private:
	CPxBone() {}
	virtual ~CPxBone() = default;
public:
	virtual HRESULT Initialize(const Json& jBone, CBone* pParent) override;

	void CreateJointsRecur(physx::PxRigidDynamic* pParentActor, const _float4x4& PivotMatrix, const _float4x4& WorldMatrix);
	void ReleaseJointsRecur();
	_matrix GetCombindMatrix_WithPivot(_fmatrix WorldMatrixInv);

	// for root bone
	void SetWorldMatrix(const _float4x4& WorldMatrix, const _float4x4& PivotMatrix);
	physx::PxRigidDynamic* GetActor() { return m_pActor; }

	void SetNotControlledWorld(const _float4x4& NotControlledWorld) { m_NotControlledWorld = NotControlledWorld; }

	static _float fDensity;
	static _float fTolerance;
	static _float fAngle;


private:
	void ReleaseActor();
private:
	physx::PxRigidDynamic* m_pActor = nullptr;
	_float4x4 m_NotControlledWorld;


public:
	static CPxBone* Create(const Json& jBone, CBone* pParent);
	virtual void Free() override;
};

END