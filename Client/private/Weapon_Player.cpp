#include "stdafx.h"
#include "Weapon_Player.h"

CWeapon_Player::CWeapon_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CScarletWeapon(pDevice, pContext)
{
}

CWeapon_Player::CWeapon_Player(const CWeapon_Player& rhs)
	:CScarletWeapon(rhs)
{
}

HRESULT CWeapon_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWeapon_Player::Initialize(void* pArg)
{
	return S_OK;
}

void CWeapon_Player::BeginTick()
{
}

void CWeapon_Player::Tick(_double TimeDelta)
{
}

void CWeapon_Player::Late_Tick(_double TimeDelta)
{
}

void CWeapon_Player::AfterPhysX()
{
}

HRESULT CWeapon_Player::Render()
{
	return S_OK;
}

HRESULT CWeapon_Player::SetUp_Components()
{
	return S_OK;
}

CWeapon_Player* CWeapon_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CWeapon_Player::Clone(void* pArg)
{
	return nullptr;
}

void CWeapon_Player::Free()
{
}
