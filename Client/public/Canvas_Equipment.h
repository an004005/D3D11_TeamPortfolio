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
	// ���� ���� ���� ����
	array<_bool, 3> m_arrItmeWindowPick = { false, false, false };		// ������ ���� ���� �ߴ���..
	array<_float, 3> m_arrOriginalPos = { 0.0f, 0.0f, 0.0f };
	array<_float, 3> m_arrAddWeaponPos = { 0.0f, 0.0f, 0.0f };

	// �ݺ����� ���鼭 �ش� ��ü�� Render�� �� Ŭ������ m_bVisible �� ������ ���� �ʵ��� �Ѵ�.
	_uint	m_iCurrentWeaponCount = { 1 };
	_uint	m_iCurrentWeaponBCount = { 1 };

	// �����ʿ� ���� ������ ���� ���� �ʿ��� ����
	_float	m_fOriginalPos = { 0.0f };
	_float	m_fAddWeaponPos = { 0.0f };
	_uint m_iPickIndex = { 0 };
	_bool m_bWeaponPick = { false }; // ���ʿ��� ���⸦ �����ϸ� true, ���⸦ ������ false

	// Tick() �� ���鼭 ��� �������� ������ �����´�.
	vector<pair<wstring, CItem_Manager::ITEMINFO>> m_vecItmeInfo;

public:
	static CCanvas_Equipment* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCanvas* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END