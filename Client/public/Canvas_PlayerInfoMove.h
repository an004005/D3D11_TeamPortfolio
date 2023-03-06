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
	void	Set_PsychokinesisGauge(const _float & fGauge, const _float & fMaxGauge);
	void	Set_PlayerHp(const _float & fHp, const _float & fMaxHp); // UITODO : BeginTick() ���� ȣ�� 

private:
	void	ChildHp();
	void	RendomTexture_Tick(const _double & dTimeDelta);
	void	ChildPsychokinesis();

private:
	_float	m_fPsychokinesisGauge = { 0.0f };

	_float	m_fPercentageHp = { 0.0f };	// ĵ������ ������ �ִ� Hp�� Hp���� ���� ������ �ִ� Hp�� �ٸ���. Hp ���� ���� ������ TimeDelta �� ���ؼ� õõ�� ������.
	_double	m_dRendomTexture_TimeAcc = { 0.0 };

private:
	_float2 m_vFontPos = { 0.0f, 0.0f };
	_float2 m_vFontScale = { 0.0f, 0.0f };

public:
	static CCanvas_PlayerInfoMove* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END