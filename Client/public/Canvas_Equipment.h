#pragma once

#include "Canvas.h"
#include "Client_Defines.h"
#include "Item_Manager.h"

BEGIN(Client)

class CCanvas_Equipment : public CCanvas
{
protected:
	CCanvas_Equipment(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCanvas_Equipment(const CCanvas_Equipment& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	BeginTick() override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void	Weaponkind_Tick(const _double & TimeDelta);
	void	RihgtInfoMove_Tick(const _double& TimeDelta);
	void	WeaponkindPick_Tick();

	void	ArrowTick();

private:
	// 왼쪽 무기 정보 저장
	array<_bool, 3> m_arrItmeWindowPick = { false, false, false };		// 목적지 까지 도달 했는지..
	array<_float, 3> m_arrOriginalPos = { 0.0f, 0.0f, 0.0f };
	array<_float, 3> m_arrAddWeaponPos = { 0.0f, 0.0f, 0.0f };

	// 반복문을 돌면서 해당 객체의 Render는 이 클래스의 m_bVisible 에 영향을 받지 않도록 한다.
	_uint	m_iCurrentWeaponCount = { 1 };
	_uint	m_iCurrentWeaponBCount = { 1 };

	// 오른쪽에 무기 정보를 띄우기 위해 필요한 변수
	_float	m_fOriginalPos = { 0.0f };
	_float	m_fAddWeaponPos = { 0.0f };
	_uint m_iPickIndex = { 0 };
	_bool m_bWeaponPick = { false }; // 왼쪽에서 무기를 선택하면 true, 무기를 닫으면 false

	// Tick() 을 돌면서 계속 아이템의 정보를 가져온다.
	vector<pair<wstring, CItem_Manager::ITEMINFO>> m_vecItmeInfo;

public:
	static CCanvas_Equipment* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END