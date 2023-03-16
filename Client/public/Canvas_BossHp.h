#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCanvas_BossHp : public CCanvas
{
protected:
	CCanvas_BossHp(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_BossHp(const CCanvas_BossHp& rhs);

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
	void	Set_BossUIRender();

	void	Set_BossHp(const _float & fHp); // 0~1 사이로 계산된 값을 던지기! currenthp/maxhp
	void	Set_BossShild(const _float & fShild);

public:
	static CCanvas_BossHp* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END