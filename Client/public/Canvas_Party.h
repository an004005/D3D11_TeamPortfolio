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
	_float	fShaderInfoIndex = { 0.0f };
	_uint	iSasMember = { 0 };	// SASMEMBER 을 알수 없기에 _uint 로 하고 형변환 했습니다.

}PICKINFO;

class CCanvas_Party : public CCanvas
{
	enum class SASMEMBER { YUITO, HANABI, TSUGUMI, GEMMA, LUCA, SEEDEN, ARASHI, KYOTO, SASMEMBER_END };

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

private:
	void	SASMemberInfo_Initialize();

	void	ChildRender_Tick();
	void	CurrentPick_Tick();
	void	PickInfo();
	void	Reserve_Tick();
	void	ReserveArrow_Tick();

	_int wrap(_int x, _int low, _int high);

private:
	PICKINFO	m_tPickIngo = {};
	SASMEMBER	 m_eSASMember = { CCanvas_Party::SASMEMBER::YUITO };

	array<PICKINFO, 5>	 m_arrSASInfo;

	// 화살표를 화면에 보이게 하기 위해서
	array<_bool, 5>	m_arrReserve_Index = { false, false, false, false, false };	// 한 번만 체크하기 위한 용도
	_uint	m_iReserve_Count = { 0 };

	// SAS
	array<PICKINFO, 5>	m_arrReserve;	// 앞면 0, 1 / 뒷면 0, 1
	_int	m_iFrontPage = { 0 };
	_int	m_iCount = { 0 };

public:
	static CCanvas_Party* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END