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

};

END