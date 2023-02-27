#include "stdafx.h"
#include "..\public\Monster.h"


CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CScarletCharacter(pDevice, pContext)
{
}

CMonster::CMonster(const CScarletCharacter& rhs)
	: CScarletCharacter(rhs)
{
}
