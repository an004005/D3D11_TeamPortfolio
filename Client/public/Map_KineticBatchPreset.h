#pragma once
#include "Base.h"
#include "Client_Defines.h"

// ������ Kinetic Object ������ �̱������� ���� (������ ����)

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

public:
	void	Set_RespawnTime(_float fRespawnTime) { m_fRespawnTime = fRespawnTime; }

private:
	void	OverlapCheck(KINETIC_BATCH& tBatch);

private:
	_float	m_fRespawnTime = 0.f;

private:
	vector<KINETIC_BATCH> m_KineticPresets;

public:
	virtual void Free() override;
};

END