#pragma once
#include "TriggerEx.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSimpleTrigger : public CTriggerEx
{
protected:
	CSimpleTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSimpleTrigger(const CSimpleTrigger& rhs);
	virtual ~CSimpleTrigger() = default;

public:
	virtual void SaveToJson(Json& json) override;
	virtual void LoadFromJson(const Json& json) override;

	virtual void Imgui_RenderProperty() override;

	virtual void TriggerInEvent(CGameObject* pObject) override;
	virtual _bool CheckTriggeredObject(CGameObject* pObject) override;

private:
	wstring m_strLayer;
	string m_strJsonPath;
	wstring m_strProtoTag;

public:
	void Free() override;
	static CSimpleTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
};

END