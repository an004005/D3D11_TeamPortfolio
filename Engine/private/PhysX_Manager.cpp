#include "stdafx.h"
#include "..\public\PhysX_Manager.h"
#include "DebugDraw.h"
#include "Graphic_Device.h"
#include "PipeLine.h"
#include "JsonStorage.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "GameUtils.h"
#include "RigidBody.h"
#include "Shader.h"
#include "VIBuffer_Line.h"

using namespace physx;

array<array<ECOLLISION_TYPE, CT_END>, CT_END> CPhysX_Manager::s_CollisionTable{};

IMPLEMENT_SINGLETON(CPhysX_Manager)

PxFilterFlags PxEngineSimulationFilterShader(
	PxFilterObjectAttributes attributes0,
	PxFilterData filterData0, 
	PxFilterObjectAttributes attributes1,
	PxFilterData filterData1,
	PxPairFlags& pairFlags,
	const void* constantBlock,
	PxU32 constantBlockSize)
{
	if (ECOLLISION_TYPE::COLL_IGNORE == CPhysX_Manager::CheckCollisionTable(static_cast<ECOLLISION_TYPE>(filterData0.word0), static_cast<ECOLLISION_TYPE>(filterData1.word0)))
	{
		return PxFilterFlag::eSUPPRESS;
	}

	if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlags();
	}

	// PxPairFlag::eNOTIFY_TOUCH_FOUND 추가하면 OnContact 실행됨
	pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND;
	return PxFilterFlags();
}

class CEngineSimulationEventCallback : public physx::PxSimulationEventCallback
{
public:
    virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
    virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;
    virtual void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override;
    virtual void onWake(physx::PxActor** actors, physx::PxU32 count) override;
    virtual void onSleep(physx::PxActor** actors, physx::PxU32 count) override;
	virtual void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override;
} g_SimulationEventCallback;

CPhysX_Manager::CPhysX_Manager()
{
	for (auto& row : s_CollisionTable)
	{
		for (auto& e : row)
		{
			e = ECOLLISION_TYPE::COLL_BLOCK;
		}
	}

	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Settings/Physx.json");
	m_bRenderDebug = json["DebugRender"];

	if (json.contains("CollisionTable"))
	{
		vector<vector<ECOLLISION_TYPE>> CollisionTable = json["CollisionTable"];
		
		for (int i = 0; i < CollisionTable.size(); ++i)
		{
			for (int j = 0; j < CollisionTable[i].size(); ++j)
			{
				s_CollisionTable[i][j] = CollisionTable[i][j];
			}
		}
	}
}

void CPhysX_Manager::Initialize()
{
	// init physx
	m_Foundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_DefaultAlloc, m_DefaultError);
	Assert(m_Foundation != nullptr);

	//m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, PxTolerancesScale(),true, m_PVD);
	m_ToleranceScale.length = 1;        // typical length of an object
	m_ToleranceScale.speed = 9.81;         // typical speed of an object, gravity*1s is a reasonable choice

#ifdef _DEBUG
	m_PVD = PxCreatePvd(*m_Foundation);
	physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	m_PVD->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, m_ToleranceScale, true, m_PVD);
	PxInitExtensions(*m_Physics, m_PVD);
#else
	m_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_Foundation, m_ToleranceScale);
#endif

	physx::PxSceneDesc sceneDesc(m_Physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	m_Dispatcher = physx::PxDefaultCpuDispatcherCreate(3);
	sceneDesc.cpuDispatcher	= m_Dispatcher;
	sceneDesc.filterShader	= PxEngineSimulationFilterShader;
	sceneDesc.broadPhaseType = physx::PxBroadPhaseType::eABP;
	sceneDesc.simulationEventCallback = &g_SimulationEventCallback;
	
	m_Scene = m_Physics->createScene(sceneDesc);

	m_pCooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_Foundation, PxCookingParams(m_ToleranceScale));

	PxCookingParams params(m_ToleranceScale);
	params.meshWeldTolerance = 0.01f;
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eWELD_VERTICES;
	m_pCooking->setParams(params);

#ifdef _DEBUG
	physx::PxPvdSceneClient* pvdClient = m_Scene->getScenePvdClient();
	if (pvdClient)
	{
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
	m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eWORLD_AXES, 1.0f);
	// m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eACTOR_AXES, 2.0f);
	// m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_EDGES, 1);
	m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1);
	// m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eCONTACT_POINT, 2);
	// m_Scene->setVisualizationParameter(physx::PxVisualizationParameter::eCONTACT_NORMAL, 2);
	// m_Scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f);
	// m_Scene->setVisualizationParameter(PxVisualizationParameter::eJOINT_LIMITS, 1.0f);

	auto pContext = CGraphic_Device::GetInstance()->GetContext();
	auto pDevice = CGraphic_Device::GetInstance()->GetDevice();
	m_pBatch = new PrimitiveBatch<VertexPositionColor>(pContext);

	m_pEffect = new BasicEffect(pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void*		pShaderByteCode;
	size_t			iShaderByteCodeSize;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeSize);
	FAILED_CHECK(pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeSize, &m_pInputLayout));

	m_pShader = CShader::Create(pDevice, pContext, L"../Bin/ShaderFiles/Shader_VtxLine.hlsl", VTXLINE_DECLARATION::Elements, VTXLINE_DECLARATION::iNumElements);
	m_pVIBuffer = CVIBuffer_Line::Create(pDevice, pContext);

#endif
	
	// create simulation
	m_Materials.emplace("Default", m_Physics->createMaterial(0.5f, 0.5f, 0.6f));
	m_Materials.emplace("SmallFriction", m_Physics->createMaterial(0.1f, 0.1f, 0.5f));
	m_Materials.emplace("NoBounce", m_Physics->createMaterial(0.5f, 0.5f, 0.3f));
	m_Materials.emplace("SmallBounce", m_Physics->createMaterial(0.5f, 0.5f, 0.1f));

	groundPlane = PxCreatePlane(*m_Physics, physx::PxPlane(0,1,0,30), *FindMaterial("Default"));
	PxShape* shape;
	groundPlane->getShapes(&shape, 1);
	shape->setSimulationFilterData(PxFilterData{ static_cast<PxU32>(CT_STATIC), 0, 0, 0 });
	shape->setQueryFilterData(PxFilterData{static_cast<PxU32>(GetCollTypeBit(CT_STATIC)), 0, 0, 0 });
	shape->setFlag(PxShapeFlag::eVISUALIZATION, false);
	m_Scene->addActor(*groundPlane);

	m_pControllerManager = PxCreateControllerManager(*m_Scene);
	m_pControllerManager->setOverlapRecoveryModule(true);
	
}

void CPhysX_Manager::Simulate(_double TimeDelta)
{
#ifdef _DEBUG
	if (CGameInstance::GetInstance()->KeyDown(DIK_F9))
		m_bRenderDebug = !m_bRenderDebug;
#endif

	m_Scene->simulate((_float)TimeDelta);
}

void CPhysX_Manager::WaitSimulate()
{
    m_Scene->fetchResults(true);
}

physx::PxMaterial* CPhysX_Manager::FindMaterial(const char* pMtrlTag)
{
	auto itr = m_Materials.find(pMtrlTag);
	Assert(itr != m_Materials.end());
	return itr->second;
}

_bool CPhysX_Manager::RayCast(const RayCastParams& params)
{
	_float3 vNormalDir{ params.vDir.x, params.vDir.y, params.vDir.z };
	vNormalDir.Normalize();

#ifdef _DEBUG
	if (params.fVisibleTime > 0.f)
		m_DebugLines.push_back(DebugLine{params.fVisibleTime, params.vOrigin, vNormalDir * params.fDistance + params.vOrigin});
#endif

	PxFilterData filterData;
	filterData.word0 = params.iTargetType;
	PxQueryFilterData queryFilterData;
	queryFilterData.data = filterData;
	queryFilterData.flags = params.queryFlags;
	if (params.bSingle)
		 queryFilterData.flags |= PxQueryFlag::eANY_HIT;

	static const PxHitFlags hitFlags = PxHitFlag::eNORMAL | PxHitFlag::ePOSITION;

	return m_Scene->raycast(
		PxVec3{ params.vOrigin.x, params.vOrigin.y, params.vOrigin.z }, 
		PxVec3{ vNormalDir.x, vNormalDir.y, vNormalDir.z },
		params.fDistance, 
		*params.rayOut, 
		hitFlags,
		queryFilterData);
}

_bool CPhysX_Manager::OverlapSphere(const SphereOverlapParams& params)
{
	const PxSphereGeometry sphere(params.fRadius);
	const PxTransform transform(PxVec3{ params.vPos.x, params.vPos.y, params.vPos.z });

#ifdef _DEBUG
	if (params.fVisibleTime > 0.f && m_bRenderDebug)
	{
		PxShape* pShape = m_Physics->createShape(sphere, *FindMaterial("Default"));
		AddDebugShape(pShape, transform, params.fVisibleTime);
	}
#endif

	PxFilterData filterData;
	filterData.word0 = params.iTargetType;
	PxQueryFilterData queryFilterData;
	queryFilterData.data = filterData;
	queryFilterData.flags = params.queryFlags;

	return m_Scene->overlap(sphere, transform, *params.overlapOut, queryFilterData);
}

_bool CPhysX_Manager::OverlapCapsule(const CapsuleOverlapParams& params)
{
	const PxCapsuleGeometry capsule(params.fRadius, params.fHalfHeight);

	static const _float4x4 RotateYMatrix = _float4x4::CreateRotationY(XMConvertToRadians(90.f));

	// capsule이 x축으로 길다. 편의상 z축 방향으로 길게 회전을 준다.
	_vector	_vLook = XMVector3TransformNormal(XMVector3Normalize(params.vLook), RotateYMatrix);

	_vector	vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), _vLook));
	_vector	vUp = XMVector3Normalize(XMVector3Cross(_vLook, vRight));
	_matrix RotMatrix(vRight, vUp, _vLook, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	Quaternion Quat = XMQuaternionRotationMatrix(RotMatrix);

	const PxTransform transform(PxVec3{ params.vPos.x, params.vPos.y, params.vPos.z }, PxQuat{Quat.x, Quat.y, Quat.z, Quat.w});

#ifdef _DEBUG
	if (params.fVisibleTime > 0.f && m_bRenderDebug)
	{
		PxShape* pShape = m_Physics->createShape(capsule, *FindMaterial("Default"));
		AddDebugShape(pShape, transform, params.fVisibleTime);
	}
#endif

	PxFilterData filterData;
	filterData.word0 = params.iTargetType;
	PxQueryFilterData queryFilterData;
	queryFilterData.data = filterData;
	queryFilterData.flags = params.queryFlags;

	return m_Scene->overlap(capsule, transform, *params.overlapOut, queryFilterData);
}

_bool CPhysX_Manager::SweepSphere(const SphereSweepParams& params)
{
	const PxSphereGeometry sphere(params.fRadius);
	const PxTransform transform(PxVec3{ params.vPos.x, params.vPos.y, params.vPos.z });
	_float3 vNormalDir{ params.vUnitDir.x, params.vUnitDir.y, params.vUnitDir.z };
	vNormalDir.Normalize();

#ifdef _DEBUG
	if ((params.fVisibleTime > 0.f || params.fDistance > 0.f) && m_bRenderDebug)
	{
		_float fSegment = 0.5f;
		if (params.fDistance <= 1.f)
			fSegment = 0.1f;

		_uint iCnt = 100;
		_float fMoveDistance = 0.f;
		PxTransform sweepTransform(transform);
		while (iCnt > 0 && fMoveDistance <= params.fDistance)
		{
			PxShape* pShape = m_Physics->createShape(sphere, *FindMaterial("Default"));
			AddDebugShape(pShape, sweepTransform, params.fVisibleTime);

			sweepTransform.p.x += vNormalDir.x * fSegment;
			sweepTransform.p.y += vNormalDir.y * fSegment;
			sweepTransform.p.z += vNormalDir.z * fSegment;
			fMoveDistance += fSegment;
			--iCnt;
		}
	}
#endif

	PxFilterData filterData;
	filterData.word0 = params.iTargetType;
	PxQueryFilterData queryFilterData;
	queryFilterData.data = filterData;
	queryFilterData.flags = params.queryFlags;
	static const PxHitFlags hitFlags = PxHitFlag::eNORMAL | PxHitFlag::ePOSITION;

	return m_Scene->sweep(
		sphere,
		transform,
		PxVec3{ vNormalDir.x, vNormalDir.y, vNormalDir.z },
		params.fDistance,
		*params.sweepOut, 
		hitFlags,
		queryFilterData);
}

_bool CPhysX_Manager::SweepCapsule(const CapsuleSweepParams& params)
{
	const PxCapsuleGeometry capsule(params.fRadius, params.fHalfHeight);

	static const _float4x4 RotateYMatrix = _float4x4::CreateRotationY(XMConvertToRadians(90.f));

	// capsule이 x축으로 길다. 편의상 z축 방향으로 길게 회전을 준다.
	_vector	_vLook = XMVector3TransformNormal(XMVector3Normalize(params.vLook), RotateYMatrix);

	_vector	vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), _vLook));
	_vector	vUp = XMVector3Normalize(XMVector3Cross(_vLook, vRight));
	_matrix RotMatrix(vRight, vUp, _vLook, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	Quaternion Quat = XMQuaternionRotationMatrix(RotMatrix);

	const PxTransform transform(PxVec3{ params.vPos.x, params.vPos.y, params.vPos.z }, PxQuat{Quat.x, Quat.y, Quat.z, Quat.w});
	_float3 vNormalDir{ params.vUnitDir.x, params.vUnitDir.y, params.vUnitDir.z };
	vNormalDir.Normalize();

#ifdef _DEBUG
	if ((params.fVisibleTime > 0.f || params.fDistance > 0.f) && m_bRenderDebug)
	{
		_float fSegment = 0.5f;
		if (params.fDistance <= 1.f)
			fSegment = 0.1f;

		fSegment = params.fDistance;

		_uint iCnt = 100;
		_float fMoveDistance = 0.f;
		PxTransform sweepTransform(transform);
		while (iCnt > 0 && fMoveDistance <= params.fDistance)
		{
			PxShape* pShape = m_Physics->createShape(capsule, *FindMaterial("Default"));
			AddDebugShape(pShape, sweepTransform, params.fVisibleTime);

			sweepTransform.p.x += vNormalDir.x * fSegment;
			sweepTransform.p.y += vNormalDir.y * fSegment;
			sweepTransform.p.z += vNormalDir.z * fSegment;
			fMoveDistance += fSegment;
			iCnt--;
		}
	}
#endif

	PxFilterData filterData;
	filterData.word0 = params.iTargetType;
	PxQueryFilterData queryFilterData;
	queryFilterData.data = filterData;
	queryFilterData.flags = params.queryFlags;
	static const PxHitFlags hitFlags = PxHitFlag::eNORMAL | PxHitFlag::ePOSITION;

	return m_Scene->sweep(
		capsule,
		transform,
		PxVec3{ vNormalDir.x, vNormalDir.y, vNormalDir.z },
		params.fDistance,
		*params.sweepOut, 
		hitFlags,
		queryFilterData);
}

_bool CPhysX_Manager::PxSweepCapsule(const PxCapsuleSweepParams& params)
{
	_float3 vNormalDir{ params.vUnitDir.x, params.vUnitDir.y, params.vUnitDir.z };
	vNormalDir.Normalize();

#ifdef _DEBUG
	if ((params.fVisibleTime > 0.f || params.fDistance > 0.f) && m_bRenderDebug )
	{
		_float fSegment = 0.5f;
		if (params.fDistance <= 1.f)
			fSegment = 0.1f;

		fSegment = params.fDistance;

		_uint iCnt = 100;
		_float fMoveDistance = 0.f;
		PxTransform sweepTransform(params.pxTransform);
		while (iCnt > 0 && fMoveDistance <= params.fDistance)
		{
			PxShape* pShape = m_Physics->createShape(params.CapsuleGeo, *FindMaterial("Default"));
			AddDebugShape(pShape, sweepTransform, params.fVisibleTime);

			sweepTransform.p.x += vNormalDir.x * fSegment;
			sweepTransform.p.y += vNormalDir.y * fSegment;
			sweepTransform.p.z += vNormalDir.z * fSegment;
			fMoveDistance += fSegment;
			iCnt--;
		}
	}
#endif

	PxFilterData filterData;
	filterData.word0 = params.iTargetType;
	PxQueryFilterData queryFilterData;
	queryFilterData.data = filterData;
	queryFilterData.flags = params.queryFlags;
	static const PxHitFlags hitFlags = PxHitFlag::eNORMAL | PxHitFlag::ePOSITION;

	return m_Scene->sweep(
		params.CapsuleGeo,
		params.pxTransform,
		PxVec3{ vNormalDir.x, vNormalDir.y, vNormalDir.z },
		params.fDistance,
		*params.sweepOut,
		hitFlags,
		queryFilterData);
}

void CPhysX_Manager::AddActor(physx::PxActor& pActor)
{
	m_Scene->addActor(pActor);
}

void CPhysX_Manager::RemoveActor(physx::PxActor& pActor)
{
	m_Scene->removeActor(pActor);
}

void CPhysX_Manager::Imgui_RenderProperty()
{
	ImGui::Checkbox("DebugDraw", &m_bRenderDebug);

	static array<string, CT_END + 1> ColliderNames{
		"E", "Player", "Monster", "PlayerAtk", "MonsterAtk", "PsychickObj", "Trigger4Player", "Trigger4Monster", "Static", "MonsterPart", "MonsterRange", "PlayerRange", "Particle"
	};

	ImGui::Checkbox("ShowTable", &m_bShowTable);
	if (m_bShowTable)
	{
	    static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
		if (ImGui::BeginTable("CollisionTable", CT_END + 1, flags))
		{
			for (auto& collName : ColliderNames)
				ImGui::TableSetupColumn(collName.c_str());
			ImGui::TableHeadersRow();

			for (int i = 0; i < CT_END; ++i)
			{
	            ImGui::TableNextColumn();
				ImGui::Text(ColliderNames[i + 1].c_str());
				for (int j = 0; j < CT_END; ++j)
				{
					ImGui::TableNextColumn();
					_bool bColl = s_CollisionTable[i][j] == ECOLLISION_TYPE::COLL_BLOCK;
					string label = "##" + to_string(i) + "_" + to_string(j);
					ImGui::Checkbox(label.c_str(), &bColl);
					if (bColl)
						s_CollisionTable[i][j] = ECOLLISION_TYPE::COLL_BLOCK;
					else
						s_CollisionTable[i][j] = ECOLLISION_TYPE::COLL_IGNORE;
				}
			}

			ImGui::EndTable();
		}
	}


	if (ImGui::Button("Save Settings"))
	{
		Json json;
		json["DebugRender"] = m_bRenderDebug;
		json["CollisionTable"] = s_CollisionTable;

		std::ofstream file("../Bin/Resources/Settings/Physx.json");
		file << json;
	}

	static _uint iRayTarget = CTB_STATIC;
	static _uint iOverlapTarget = CTB_PLAYER;
	static _uint iSweepTarget = CTB_STATIC | CTB_PLAYER;

	if (CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_LB))
	{
		_float4 vOrigin;
		_float4 vDir;
		CGameUtils::GetPickingRay(vOrigin, vDir);

		const _uint bufferSize = 6;
		PxRaycastHit hitBuffer[bufferSize];
		PxRaycastBuffer t(hitBuffer, bufferSize);

		RayCastParams params;
		params.rayOut = &t;
		params.vOrigin = vOrigin;
		params.vDir = vDir;
		params.fDistance = 1000.f;
		params.iTargetType = iRayTarget;
		params.bSingle = false;

		if (RayCast(params))
		{
			IM_LOG("Hit Ray");
			for (int i = 0; i < t.getNbAnyHits(); ++i)
			{
				auto p = t.getAnyHit(i);

				_float4 vPos{ p.position.x, p.position.y, p.position.z, 1.f };

				PxSweepHit hitBuffer[4];
				PxSweepBuffer overlapOut(hitBuffer, 4);
				CapsuleSweepParams params2;
				params2.sweepOut = &overlapOut;
				params2.fRadius = 0.5f;
				params2.fHalfHeight = 2.f;
				params2.vLook = _float3{ 0.f, 0.f, 1.f };
				params2.vPos = vPos;
				params2.vUnitDir = _float3{ 1.f, 0.f, 0.f };
				params2.fDistance = 3.f;
				params2.iTargetType = iSweepTarget;

				if (SweepCapsule(params2))
				{
					for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
					{
						if (overlapOut.getAnyHit(i).actor)
							IM_LOG("Sweeped");
						CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(overlapOut.getAnyHit(i).actor);
					}
				}
			}
		}
	}

	if (CGameInstance::GetInstance()->KeyDown(CInput_Device::DIM_RB))
	{
		_float4 vOrigin;
		_float4 vDir;
		CGameUtils::GetPickingRay(vOrigin, vDir);

		PxRaycastBuffer out;
		RayCastParams params;
		params.rayOut = &out;
		params.vOrigin = vOrigin;
		params.vDir = vDir;
		params.fDistance = 1000.f;
		params.iTargetType = iRayTarget;

		if (RayCast(params))
		{
			IM_LOG("Hit Ray");
			for (int i = 0; i < out.getNbAnyHits(); ++i)
			{
				auto p = out.getAnyHit(i);
				_float4 vPos{ p.position.x, p.position.y, p.position.z, 1.f };

				PxOverlapHit hitBuffer[4];
				PxOverlapBuffer overlapOut(hitBuffer, 4);
				SphereOverlapParams params2;
				params2.overlapOut = &overlapOut;
				params2.fRadius = 1.f;
				params2.vPos = vPos;
				params2.iTargetType = iOverlapTarget;

				if (OverlapSphere(params2))
				{
					for (int i = 0; i < overlapOut.getNbAnyHits(); ++i)
					{
						if (overlapOut.getAnyHit(i).actor)
							IM_LOG("Overlap");
						CGameObject* pCollidedObject = CPhysXUtils::GetOnwer(overlapOut.getAnyHit(i).actor);
					}
				}
			}
		}
	}

}

ECOLLISION_TYPE CPhysX_Manager::CheckCollisionTable(ECOLLISION_TYPE e1, ECOLLISION_TYPE e2)
{
	return s_CollisionTable[static_cast<_uint>(e1)][static_cast<_uint>(e2)];
}

#ifdef _DEBUG
void CPhysX_Manager::DebugRender(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (m_bRenderDebug == false)
	{
		for (auto& debugShape : m_DebugShapes)
		{
			RemoveActor(*debugShape.pActor);
			debugShape.pActor->detachShape(*debugShape.pShape);
			debugShape.pActor->release();
			debugShape.pShape->release();
		}
		m_DebugLines.clear();
		m_DebugShapes.clear();
		return;
	}

	m_pEffect->SetWorld(XMMatrixIdentity());

	CPipeLine*		pPipeLine = CPipeLine::GetInstance();

	m_pEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));
	m_pEffect->Apply(pContext);
	pContext->IASetInputLayout(m_pInputLayout);
	m_pBatch->Begin();
	for (auto& line : m_DebugLines)
	{
		line.fLife -= TIME_DELTA;
		DX::DrawRay(m_pBatch, line.vOrigin, line.vDirection, false, XMLoadFloat4(&_float4(1.f, 0.f, 0.f, 0.f)));
	}
	m_pBatch->End();

	FAILED_CHECK(m_pShader->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShader->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));
	const physx::PxRenderBuffer& rb = m_Scene->getRenderBuffer();
	vector<VTXLINE> lines;
	lines.reserve(2000);
	for(physx::PxU32 i=0; i < rb.getNbLines(); i++)
	{
		VTXLINE vtxLine_0;
		VTXLINE vtxLine_1;

	    const physx::PxDebugLine& line = rb.getLines()[i];
		// const _vector vOrigin = XMVectorSet(line.pos0.x, line.pos0.y, line.pos0.z, 0.f);
		// const _vector vDest = XMVectorSet(line.pos1.x, line.pos1.y, line.pos1.z, 0.f);
		// const _vector vDirection = vDest - vOrigin;

		vtxLine_0.vPosition = _float3(line.pos0.x, line.pos0.y, line.pos0.z);
		vtxLine_1.vPosition = _float3(line.pos1.x, line.pos1.y, line.pos1.z);
		_float4 vColor;
		vColor.w = static_cast<_float>((line.color0 >> 24) & 255);
		vColor.x = static_cast<_float>((line.color0 >> 16) & 255);
		vColor.y = static_cast<_float>((line.color0 >> 8) & 255);
		vColor.z = static_cast<_float>(line.color0 & 255);
		vtxLine_0.vColor = vColor;
		vtxLine_1.vColor = vColor;
		lines.push_back(vtxLine_0);
		lines.push_back(vtxLine_1);
		if (lines.size() >= 2000 || i == rb.getNbLines() - 1)
		{
			m_pVIBuffer->SetLines(lines);
			m_pShader->Begin(0);
			m_pVIBuffer->Render();
			lines.clear();
		}
	}

	m_DebugLines.remove_if([](const DebugLine& line)
	{
		return line.fLife < 0.f;
	});

	for (auto& debugShape : m_DebugShapes)
	{
		debugShape.fLife -= TIME_DELTA;
		if (debugShape.fLife <= 0.f)
		{
			RemoveActor(*debugShape.pActor);
			debugShape.pActor->detachShape(*debugShape.pShape);
			debugShape.pActor->release();
			debugShape.pShape->release();
			debugShape.pShape = nullptr;
			debugShape.pActor = nullptr;
		}
	}
	m_DebugShapes.remove_if([](const DebugShape& shape)
	{
		return shape.pActor == nullptr;
	});
}

void CPhysX_Manager::AddDebugShape(physx::PxShape* pShape, PxTransform pxTransform, _float fLife)
{
	DebugShape debugShape;
	debugShape.fLife = fLife;
	debugShape.pShape = pShape;

	debugShape.pShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
	debugShape.pShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);
	debugShape.pShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
	debugShape.pShape->setFlag(physx::PxShapeFlag::eVISUALIZATION, true);

	debugShape.pActor = m_Physics->createRigidStatic(pxTransform);
	debugShape.pActor->attachShape(*debugShape.pShape);
	AddActor(*debugShape.pActor);

	m_DebugShapes.push_back(debugShape);
}
#endif

void CPhysX_Manager::Free()
{
#ifdef _DEBUG
	for (auto& debugShape : m_DebugShapes)
	{
		RemoveActor(*debugShape.pActor);
		debugShape.pActor->detachShape(*debugShape.pShape);
		debugShape.pActor->release();
		debugShape.pShape->release();
	}
#endif

	m_pCooking->release();
	m_pControllerManager->release();
	m_Scene->release();
	m_Dispatcher->release();
	m_Physics->release();

#ifdef _DEBUG
	PxCloseExtensions();
	m_PVD->disconnect();
	m_PVD->getTransport()->release();
	m_PVD->release();

	Safe_Release(m_pInputLayout);
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pShader);
	Safe_Release(m_pVIBuffer);
#endif

	m_Foundation->release();
}

physx::PxMat44 CPhysXUtils::ToFloat4x4(const _float4x4& mat)
{
	physx::PxMat44 out;
	memcpy(&out.column0, &mat.m[0], sizeof(_float4));
	memcpy(&out.column1, &mat.m[1], sizeof(_float4));
	memcpy(&out.column2, &mat.m[2], sizeof(_float4));
	memcpy(&out.column3, &mat.m[3], sizeof(_float4));
	return out;
}

_float4x4 CPhysXUtils::ToFloat4x4(const physx::PxMat44& mat)
{
	_float4x4 out;
	memcpy(&out.m[0], &mat.column0, sizeof(_float4));
	memcpy(&out.m[1], &mat.column1, sizeof(_float4));
	memcpy(&out.m[2], &mat.column2, sizeof(_float4));
	memcpy(&out.m[3], &mat.column3, sizeof(_float4));
	return out;
}

void CPhysXUtils::AddForceAtLocalPos(physx::PxRigidBody& body, const physx::PxVec3& force, const physx::PxVec3& pos,
	physx::PxForceMode::Enum mode, bool wakeup)
{
	//transform pos to world space
	const physx::PxVec3 globalForcePos = body.getGlobalPose().transform(pos);

	const physx::PxTransform globalPose = body.getGlobalPose();
	const physx::PxVec3 centerOfMass = globalPose.transform(body.getCMassLocalPose().p);

	const physx::PxVec3 torque = (globalForcePos - centerOfMass).cross(force);
	body.addForce(force, mode, wakeup);
	body.addTorque(torque, mode, wakeup);
}

CGameObject* CPhysXUtils::GetOnwer(physx::PxActor* pActor)
{
	// userData에는 Physx를 사용하는 컴포넌트의 주소가 항상 들어간다.
	if (pActor == nullptr || pActor->userData == nullptr)
		return nullptr;
	CComponent* pPxCom = static_cast<CComponent*>(pActor->userData);
	return pPxCom->TryGetOwner();
}

CComponent* CPhysXUtils::GetComponent(physx::PxActor* pActor)
{
	if (pActor == nullptr || pActor->userData == nullptr)
		return nullptr;
	return static_cast<CComponent*>(pActor->userData);
}

/* **************************************
 * CEngineSimulationEventCallback
 ***************************************/
void CEngineSimulationEventCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	IM_LOG("Contacted");
	
	{
		CComponent* pPxCom = static_cast<CComponent*>(pairHeader.actors[0]->userData);
		if (CRigidBody* pRigid = dynamic_cast<CRigidBody*>(pPxCom))
		{
			pRigid->CallOnTriggerIn(CPhysXUtils::GetOnwer(pairHeader.actors[1]));
		}
	}

	{
		CComponent* pPxCom = static_cast<CComponent*>(pairHeader.actors[1]->userData);
		if (CRigidBody* pRigid = dynamic_cast<CRigidBody*>(pPxCom))
		{
			pRigid->CallOnTriggerIn(CPhysXUtils::GetOnwer(pairHeader.actors[0]));
		}
	}

}

void CEngineSimulationEventCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	CComponent* pPxCom = static_cast<CComponent*>(pairs->triggerActor->userData);
	if (pPxCom == nullptr)
		return;

	CRigidBody* pRigid = dynamic_cast<CRigidBody*>(pPxCom);
	if (pRigid == nullptr)
		return;

	switch (pairs->status)
	{
	case PxPairFlag::eNOTIFY_TOUCH_FOUND:
		IM_LOG("Trigger In!");
		pRigid->CallOnTriggerIn(CPhysXUtils::GetOnwer(pairs->otherActor));
		break;
	case PxPairFlag::eNOTIFY_TOUCH_LOST:
		IM_LOG("Trigger Out!");
		pRigid->CallOnTriggerOut(CPhysXUtils::GetOnwer(pairs->otherActor));
		break;

	default: ;
		IM_LOG("Non Handled Trigger %d", (_int)pairs->status);
	}
}

void CEngineSimulationEventCallback::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
{
    // Handle constraint break events
}

void CEngineSimulationEventCallback::onWake(physx::PxActor** actors, physx::PxU32 count)
{
    // Handle wake events
}

void CEngineSimulationEventCallback::onSleep(physx::PxActor** actors, physx::PxU32 count)
{
    // Handle sleep events
}

void CEngineSimulationEventCallback::onAdvance(const physx::PxRigidBody* const* bodyBuffer,
	const physx::PxTransform* poseBuffer, const physx::PxU32 count)
{
}

