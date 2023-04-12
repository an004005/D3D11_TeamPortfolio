#pragma once
#include "Base.h"
#include "Client_Defines.h"

// 보스방 Kinetic Object 정보를 싱글톤으로 관리 (재사용을 위함)

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)
class CMapKinetic_Object;

typedef struct tKineticBatch
{
	string strJsonPath;
	string strLayer;
	string strProtoTag;
	_float4x4 matWorldMtx;

	class CMapKinetic_Object* pKineticObj = nullptr;

	_float fCoolTime = 0.f;

}KINETIC_BATCH;

class CMap_KineticBatchPreset final : public CBase
{
	DECLARE_SINGLETON(CMap_KineticBatchPreset)

private:
	CMap_KineticBatchPreset();
	virtual ~CMap_KineticBatchPreset() = default;

public:
	HRESULT	Initialize(Json json);
	void	Tick(_double TimeDelta);
	void Clear();

private:
	void	OverlapCheck(KINETIC_BATCH& tBatch);

private:
	vector<KINETIC_BATCH> m_KineticPresets;

public:
	virtual void Free() override;
};

END