#pragma once
#include "Base.h"

BEGIN(Engine)

class CPhysX_Manager final : public CBase
{
	DECLARE_SINGLETON(CPhysX_Manager)
private:
	CPhysX_Manager();
	virtual ~CPhysX_Manager() = default;

public:
	void Initialize();
	void Simulate(_double TimeDelta);
	void WaitSimulate();

	physx::PxPhysics* GetPhysics() { return m_Physics; }
	physx::PxMaterial* FindMaterial(const char* pMtrlTag);
	physx::PxScene* GetScene() { return m_Scene; }
	physx::PxControllerManager* GetControllerManager() { return m_pControllerManager; }
	physx::PxCooking* GetCooking() { return m_pCooking; }

	/*
	 * hit 결과 여러개 받기위해서는 아래 과정으로 PxRaycastBuffer를 만들고 rayOut파라미터로 넣어준다. 아래과정 안거친 PxRaycastBuffer는 결과를 1개만 받을 수 있음
	 * const _uint bufferSize = 8;        // hit 버퍼 개수
		PxRaycastHit hitBuffer[bufferSize];  // hit 버퍼
		PxRaycastBuffer buf(hitBuffer, bufferSize); // hit 버퍼로 초기화한 PxRaycastBuffer를 넣어준다.
	 */
	_bool RayCast(const RayCastParams& params);

	/* overlap도 마찬가지로 여러개 받으려면 아래 방법으로 PxOverlapBuffer 만들어준다.
	 * 주의 : 최소 1 이상의 버퍼 사이즈 필요
	 *const _uint bufferSize = 8;
	 *PxOverlapHit hitBuffer[bufferSize];
	 *PxOverlapBuffer buf(hitBuffer, bufferSize);
	 */
	_bool OverlapSphere(const SphereOverlapParams& params);
	_bool OverlapCapsule(const CapsuleOverlapParams& params);

	// sweep 도 overlap과 마찬가지
	_bool SweepSphere(const SphereSweepParams& params);
	_bool SweepCapsule(const CapsuleSweepParams& params);
	_bool PxSweepCapsule(const PxCapsuleSweepParams& params);

	void AddActor(physx::PxActor& pActor);
	void RemoveActor(physx::PxActor& pActor);

	void Imgui_RenderProperty();

	static ECOLLISION_TYPE CheckCollisionTable(ECOLLISION_TYPE e1, ECOLLISION_TYPE e2);

#ifdef _DEBUG
	void DebugRender(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void AddDebugShape(physx::PxShape* pShape, physx::PxTransform pxTransform, _float fLife);
#endif

private:
	physx::PxDefaultAllocator m_DefaultAlloc;
	physx::PxDefaultErrorCallback m_DefaultError;
	physx::PxFoundation* m_Foundation = nullptr;
	physx::PxPhysics* m_Physics = nullptr;
	physx::PxDefaultCpuDispatcher* m_Dispatcher = nullptr;
	physx::PxScene* m_Scene = nullptr;
	physx::PxTolerancesScale m_ToleranceScale;
	physx::PxCooking* m_pCooking = nullptr;

	physx::PxControllerManager* m_pControllerManager = nullptr;

	cmap<physx::PxMaterial*> m_Materials;

	static array<array<ECOLLISION_TYPE, CT_END>, CT_END> s_CollisionTable;

	_bool m_bRenderDebug = true;
	_bool m_bShowTable = false;

#ifdef _DEBUG
	struct DebugLine
	{
		_float fLife;
		_float4 vOrigin;
		_float4 vDirection;
	};

	struct DebugShape
	{
		_float fLife;
		physx::PxRigidStatic* pActor;
		physx::PxShape* pShape;
	};
	list<DebugLine> m_DebugLines;
	list<DebugShape> m_DebugShapes;

	physx::PxRigidStatic* groundPlane;
	physx::PxPvd* m_PVD = nullptr;

	PrimitiveBatch<VertexPositionColor>*				m_pBatch = nullptr;
	BasicEffect*										m_pEffect = nullptr;
	ID3D11InputLayout*									m_pInputLayout = nullptr;	
#endif


public:
	virtual void Free() override;
};

class ENGINE_DLL CPhysXUtils
{
public:
	static physx::PxMat44 ToFloat4x4(const _float4x4& mat);
	static _float4x4 ToFloat4x4(const physx::PxMat44& mat);
	static void AddForceAtLocalPos(physx::PxRigidBody& body, const physx::PxVec3& force,
		const physx::PxVec3& pos, physx::PxForceMode::Enum mode, bool wakeup = true);
	static class CGameObject* GetOnwer(physx::PxActor* pActor);
	static class CComponent* GetComponent(physx::PxActor* pActor);
};

END