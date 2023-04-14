#pragma once
#include "TriggerEx.h"
#include "Client_Defines.h"

BEGIN(Client)

class CAlarm_Trigger : public CTriggerEx
{
protected:
	CAlarm_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAlarm_Trigger(const CAlarm_Trigger& rhs);
	virtual ~CAlarm_Trigger() = default;

public:
	virtual void SaveToJson(Json& json) override;
	virtual void LoadFromJson(const Json& json) override;

	virtual void Imgui_RenderProperty() override;

	virtual void TriggerInEvent(CGameObject* pObject) override;
	virtual _bool CheckTriggeredObject(CGameObject* pObject) override;

private:
	LEVEL	m_eLevel = LEVEL::LEVEL_END;
	wstring	m_wsName = L"";

public:
	void Free() override;
	static CAlarm_Trigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
};


END