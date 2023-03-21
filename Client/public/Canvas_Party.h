#pragma once

#include "Canvas.h"
#include "Client_Defines.h"

BEGIN(Client)

typedef struct tagPickInfo
{
	_tchar* szName = { L"" };
	_tchar* szSASNumber = { L"" };
	_uint	iLevel = { 0 };
	_uint	iExp = { 0 };
	_uint	iMaxExp = { 0 };
	_uint	iRemainingExp = { 0 };
	_uint	iBondLevel = { 0 };
	_uint	iHP = { 0 };
	_uint	iMaxHP = { 0 };
	_uint	iSprbrPower = { 0 };
	_uint	iAttack = { 0 };
	_uint	iDefense = { 0 };

}PICKINFO;

enum SASMEMBER { YUITO, HANABI, TSUGUMI, SASMEMBER_END };

class CCanvas_Party : public CCanvas
{
protected:
	CCanvas_Party(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_Party(const CCanvas_Party& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

private:
	void	ChildRender_Tick();
	void	CurrentPick_Tick();
	void	PickInfo();

private:
	PICKINFO	m_tPickIngo = {};
	SASMEMBER	m_eSASMember = { YUITO };

	_float2	m_vPosition = { 0.0f, 0.0f };	// Test

public:
	static CCanvas_Party* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END