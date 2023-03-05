#pragma once
#include "Client_Defines.h"
#include "ScarletCharacter.h"

BEGIN(Engine)

class CMonster abstract : public CScarletCharacter
{
protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CScarletCharacter& rhs);
	virtual ~CMonster() = default;

protected:
	_uint m_iHP = 100;
	_uint m_iMaxHP = 100;

	_uint m_iStamina = 100;
	_uint m_iMaxStamina = 100;

	_bool m_bDead = false;

};

END