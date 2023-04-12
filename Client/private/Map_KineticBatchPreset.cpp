#include "stdafx.h"
#include "Map_KineticBatchPreset.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "MapKinetic_Object.h"

#include "GameUtils.h"
#include "JsonStorage.h"
#include "PhysX_Manager.h"

IMPLEMENT_SINGLETON(CMap_KineticBatchPreset)

CMap_KineticBatchPreset::CMap_KineticBatchPreset()
{
}

HRESULT CMap_KineticBatchPreset::Initialize(Json json)
{
	Clear();
	// 싱글톤 사용할 곳에서 Json 경로를 던져준다.
	// 예시)
	//Json kineticJson = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Batch/BatchFiles/FinalStage/Kinetic_Normal.json");
	
	Json kineticJson = json;	
	m_KineticPresets.reserve(kineticJson["BatchObjs"].size());

	for (auto batchObj : kineticJson["BatchObjs"])
	{
		KINETIC_BATCH tBatch;

		tBatch.strJsonPath = batchObj["Preset"]["JsonPath"];
		tBatch.strLayer = batchObj["Preset"]["Layer"];
		tBatch.strProtoTag = batchObj["Preset"]["ProtoTag"];
		tBatch.matWorldMtx = batchObj["WorldMatrix"];

		Assert(tBatch.strJsonPath.empty() == false);

		Json KineticPresetJson = CJsonStorage::GetInstance()->FindOrLoadJson(tBatch.strJsonPath);
		auto pKineticObj = dynamic_cast<CMapKinetic_Object*>(CGameInstance::GetInstance()->Clone_GameObject_Get(s2ws(tBatch.strLayer).c_str(), s2ws(tBatch.strProtoTag).c_str(), &KineticPresetJson));
		Assert(pKineticObj != nullptr);
		pKineticObj->GetTransform()->Set_WorldMatrix(tBatch.matWorldMtx);
		tBatch.pKineticObj = pKineticObj;

		m_KineticPresets.push_back(tBatch);
	}

	return S_OK;
}

void CMap_KineticBatchPreset::Tick(_double TimeDelta)
{
	for (auto& iter : m_KineticPresets)
	{
		if (iter.pKineticObj == nullptr)
		{
			iter.fCoolTime += TimeDelta;

			if (iter.fCoolTime >= 5.f)
			{
				OverlapCheck(iter);
			}
		}
		else if (CGameInstance::GetInstance()->Check_ObjectAlive(iter.pKineticObj) == false)
		{
			iter.pKineticObj = nullptr;
		}
	}
}

void CMap_KineticBatchPreset::Clear()
{
	m_KineticPresets.clear();
}

void CMap_KineticBatchPreset::OverlapCheck(KINETIC_BATCH& tBatch)
{
	physx::PxOverlapHit hitBuffer[5];
	physx::PxOverlapBuffer overlapOut(hitBuffer, 5);

	SphereOverlapParams tParams;
	tParams.overlapOut = &overlapOut;
	tParams.fRadius = 2.f;
	tParams.vPos = { tBatch.matWorldMtx.Translation() };
	tParams.fVisibleTime = 1.f;
	tParams.iTargetType = CTB_PLAYER | CTB_MONSTER | CTB_MONSTER_PART | CTB_PSYCHICK_OBJ;
	tParams.queryFlags = physx::PxQueryFlag::eDYNAMIC;

	if (CGameInstance::GetInstance()->OverlapSphere(tParams))
	{
		tBatch.fCoolTime -= 2.5f;
		return;
	}
	else
	{
		Json KineticPresetJson = CJsonStorage::GetInstance()->FindOrLoadJson(tBatch.strJsonPath);
		auto pKineticObj = dynamic_cast<CMapKinetic_Object*>(CGameInstance::GetInstance()->Clone_GameObject_Get(s2ws(tBatch.strLayer).c_str(), s2ws(tBatch.strProtoTag).c_str(), &KineticPresetJson));
		Assert(pKineticObj != nullptr);
		pKineticObj->GetTransform()->Set_WorldMatrix(tBatch.matWorldMtx);
		tBatch.pKineticObj = pKineticObj;
		tBatch.fCoolTime = 0.f;
	}
}

void CMap_KineticBatchPreset::Free()
{
	Clear();
}
