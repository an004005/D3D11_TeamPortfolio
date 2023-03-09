#include "stdafx.h"
#include "..\public\BdLm_Controller.h"
#include "BuddyLumi.h"
#include "FSMComponent.h"

CBdLm_Controller::CBdLm_Controller(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CAIController(pDevice, pContext)
{
}

CBdLm_Controller::CBdLm_Controller(const CBdLm_Controller & rhs)
	: CAIController(rhs)
{
}
