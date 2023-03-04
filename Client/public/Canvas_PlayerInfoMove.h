#pragma once

#include "Client_Defines.h"
#include "Canvas.h"

BEGIN(Client)

class CCanvas_PlayerInfoMove : public CCanvas
{
protected:
	CCanvas_PlayerInfoMove(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_PlayerInfoMove(const CCanvas_PlayerInfoMove& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	BeginTick() override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

public:
	void	Set_PlayerHp(const _float & fHp) {
		ChildHp(fHp);
	}
	void	Set_PlayerMaxHp(const _float & fMaxHp) {
		ChildMaxHp(fMaxHp);
	}

private:
	void	StartAndEndHp();
	void	ChildHp(const _float & fHp);
	void	ChildMaxHp(const _float & fMaxHp);

public:
	static CCanvas_PlayerInfoMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END