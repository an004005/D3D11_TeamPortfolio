#pragma once
#include "TriggerEx.h"
#include "Client_Defines.h"
#include "Canvas_Tutorial.h"

BEGIN(Client)

class CTipTrigger : public CTriggerEx
{
protected:
	CTipTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTipTrigger(const CTipTrigger& rhs);
	virtual ~CTipTrigger() = default;

public:
	virtual void SaveToJson(Json& json) override;
	virtual void LoadFromJson(const Json& json) override;

	virtual void Imgui_RenderProperty() override;

	virtual void TriggerInEvent(CGameObject* pObject) override;
	virtual _bool CheckTriggeredObject(CGameObject* pObject) override;

private:
	CCanvas_Tutorial::TUTORIAL m_eTutorialType = CCanvas_Tutorial::TUTORIAL_END;

public:
	void Free() override;
	static CTipTrigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
};


END