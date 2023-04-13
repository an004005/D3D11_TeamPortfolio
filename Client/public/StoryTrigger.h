#pragma once
#include "TriggerEx.h"
#include "Client_Defines.h"

BEGIN(Client)

class CStoryTrigger : public CTriggerEx
{
protected:
	CStoryTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStoryTrigger(const CStoryTrigger& rhs);
	virtual ~CStoryTrigger() = default;

public:
	virtual void SaveToJson(Json& json) override;
	virtual void LoadFromJson(const Json& json) override;

	virtual void Imgui_RenderProperty() override;

	virtual void TriggerInEvent(CGameObject* pObject) override;
	virtual _bool CheckTriggeredObject(CGameObject* pObject) override;

private:
	_bool	m_bTalk = { false };	// true : MainTalk / false : LeftTalk
	_int		m_iQuest = { -1 };
	vector<_int>	m_vecTalkIndex;

public:
	void Free() override;
	static CStoryTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
};


END