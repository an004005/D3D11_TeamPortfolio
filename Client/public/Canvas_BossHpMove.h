#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_BossHpMove : public CCanvas
{
protected:
	CCanvas_BossHpMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_BossHpMove(const CCanvas_BossHpMove& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	void	Set_BossHp(const _float & fHp); // 0~1 ���̷� ���� ���� ������! currenthp/maxhp
	void	Set_BossShild(const _float & fShild);

private:
	class CCanvas_BossHp* m_pCanvas_BossHp = { nullptr };
	class CCanvas_Alarm* m_pUI_Alarm = { nullptr };

private:
	_float m_fCurrentHp = { 0.0f };

public:
	static CCanvas_BossHpMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END