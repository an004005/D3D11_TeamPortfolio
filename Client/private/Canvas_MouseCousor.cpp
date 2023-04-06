#include "stdafx.h"
#include "..\public\Canvas_MouseCousor.h"
#include "GameInstance.h"

#include "JsonStorage.h"

CCanvas_MouseCousor::CCanvas_MouseCousor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_MouseCousor::CCanvas_MouseCousor(const CCanvas_MouseCousor& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_MouseCousor::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCanvas_MouseCousor::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;
	return S_OK;
}

void CCanvas_MouseCousor::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	if (CGameInstance::GetInstance()->KeyDown(DIK_0) && LEVEL_NOW != LEVEL_FINAL_STAGE)
	{
		//assert(pCanvas != nullptr && "Failed to Cloned : Canvas_SAMouseLeft"); // üũ �ϱ�!
		 
		// ������ : ���콺 ���� Ű UI
		//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SAMouseLeft.json");
		//CGameInstance::GetInstance()->Clone_GameObject(L"Layer_Test", L"Canvas_SAMouseLeft", &json);

		// ö�� ȸ�� : ���콺 ������ UI
		//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SARebar.json");
		//CGameInstance::GetInstance()->Clone_GameObject(L"Layer_Test", L"Canvas_SARebar", &json);

		// ��â�� : ó���� Canvas_SAGragting_S ��� ���߿� Canvas_SAGragting_Go ���
		//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SAGragting_S.json");
		//CGameInstance::GetInstance()->Clone_GameObject(L"Layer_Test", L"Canvas_SAGragting_S", &json);
		//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SAGragting_Go.json");
		//CGameInstance::GetInstance()->Clone_GameObject(L"Layer_Test", L"Canvas_SAGragting_Go", &json);
	
		// �����̳� : ó���� Canvas_SAGragting_S ��� ���߿� Canvas_SAGragting_Down ���
		//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SAGragting_S.json");
		//CGameInstance::GetInstance()->Clone_GameObject(L"Layer_Test", L"Canvas_SAGragting_S", &json);
		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SAContainer_Down.json");
		CGameInstance::GetInstance()->Clone_GameObject(L"Layer_Test", L"Canvas_SAContainer_Down", &json);
	}
}

CCanvas_MouseCousor* CCanvas_MouseCousor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCanvas_MouseCousor* pInstance = new CCanvas_MouseCousor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_MouseCousor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas* CCanvas_MouseCousor::Clone(void* pArg)
{
	CCanvas_MouseCousor* pInstance = new CCanvas_MouseCousor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_MouseCousor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_MouseCousor::Free()
{
	CCanvas::Free();

}