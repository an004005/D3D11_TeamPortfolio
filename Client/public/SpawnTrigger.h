#pragma once
#include "TriggerEx.h"
#include "Client_Defines.h"
#include "HelperClasses.h"

BEGIN(Client)
class CEnemy;

struct EnemyWithPos
{
	string strEnemyTag;
	ENEMY_STAT tStat;
	_float4x4 WorldMatrix;
};

class CSpawnTrigger : public CTriggerEx
{
protected:
	CSpawnTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSpawnTrigger(const CSpawnTrigger& rhs);
	virtual ~CSpawnTrigger() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void SaveToJson(Json& json) override;
	virtual void LoadFromJson(const Json& json) override;

	virtual void Imgui_RenderProperty() override;

	virtual void TriggerInEvent(CGameObject* pObject) override;
	virtual _bool CheckTriggeredObject(CGameObject* pObject) override;

	_bool IsStarted() const { return m_bStart; }

private:
	vector<list<EnemyWithPos>> m_SpawnEnemies;
	vector<list<CEnemy*>> m_WaitingEnemies;
	_uint m_iSpawnOrder = 0;
	_bool m_bStart = false;

	class CInvisibleWall* m_pWall = nullptr;
	CDoOnce m_WallDeActiveOnce;

public:
	void Free() override;
	static CSpawnTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;

};

END