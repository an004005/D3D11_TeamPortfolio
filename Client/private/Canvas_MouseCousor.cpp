#include "stdafx.h"
#include "..\public\Canvas_MouseCousor.h"
#include "GameInstance.h"

#include "JsonStorage.h"
#include "Canvas_SAMouseLeft.h"
#include "Canvas_SARebar.h"
#include "Canvas_SAGragting_S.h"
#include "Canvas_SAGragting_Go.h"
#include "Canvas_SAContainer_Down.h"
#include "Canvas_MainTalk.h"
#include "Canvas_LeftTalk.h"
#include "Canvas_Alarm.h"
#include "Canvas_BossHpMove.h"
#include "PlayerInfoManager.h"
#include "Item_Manager.h"
#include "Canvas_BossHpMove.h"

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

	//// ���� ���� ü�¹�
	//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_BossHpMove.json");
	//m_pCanvas_BossHpMove = dynamic_cast<CCanvas_BossHpMove*>(CGameInstance::GetInstance()->Clone_GameObject_Get(TEXT("Layer_Test"), L"Canvas_BossHpMove", &json));
	//assert(m_pCanvas_BossHpMove != nullptr && "Failed to Clone : CCanvas_BossHpMove");


	return S_OK;
}

void CCanvas_MouseCousor::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	//if (LEVEL_NOW != LEVEL_UI) return;

	// ��� �� �����Ǵ� �ֵ�
	if (CGameInstance::GetInstance()->KeyDown(DIK_0))
	{
		//assert(pCanvas != nullptr && "Failed to Cloned : Canvas_SAMouseLeft"); // üũ �ϱ�!
		 
		// ������ : ���콺 ���� Ű UI
		//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SAMouseLeft.json");
		//CCanvas_SAMouseLeft* pCanvas_SAMouseLeft = dynamic_cast<CCanvas_SAMouseLeft*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_SAMouseLeft", &json));
		//pCanvas_SAMouseLeft->Set_Intput();

		// ö�� ȸ�� : ���콺 ������ UI
		//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SARebar.json");
		//CCanvas_SARebar* pCanvas_SARebar = dynamic_cast<CCanvas_SARebar*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_SARebar", &json));
		//pCanvas_SARebar->Set_Intput();

		// ��â�� : ó���� Canvas_SAGragting_S ��� ���߿� Canvas_SAGragting_Go ���
		//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SAGragting_S.json");
		//CCanvas_SAGragting_S* pCanvas_SAGragting_S = dynamic_cast<CCanvas_SAGragting_S*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_SAGragting_S", &json));
		//pCanvas_SAGragting_S->Set_Intput();
		//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SAGragting_Go.json");
		//m_pCanvas_SAGragting_Go = dynamic_cast<CCanvas_SAGragting_Go*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_SAGragting_Go", &json));
	
		// �����̳� : ó���� Canvas_SAGragting_S ��� ���߿� Canvas_SAGragting_Down ���
		//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SAGragting_S.json");
		//CGameInstance::GetInstance()->Clone_GameObject(L"Layer_Test", L"Canvas_SAGragting_S", &json);
		//json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_SAContainer_Down.json");
		//m_pCanvas_SAContainer_Down = dynamic_cast<CCanvas_SAContainer_Down*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_SAContainer_Down", &json));

		/////////////////////////////////////////////////////
		
		// �� �̸�
		//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_Alarm.json");
		//m_pCanvas_Alarm = dynamic_cast<CCanvas_Alarm*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_Alarm", &json));
		//m_pCanvas_Alarm->Set_MapName();

		CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage = 9000;


		Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_BossHpMove.json");
		CCanvas_BossHpMove* pBossHpMove = dynamic_cast<CCanvas_BossHpMove*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_BossHpMove", &json));
		pBossHpMove->Set_BossHp(1.0f);
		pBossHpMove->Set_BossShild(1.0f);
	}

	// ������ ���� �ϴ� �ֵ�
	if (CGameInstance::GetInstance()->KeyDown(DIK_9))
	{
		//// ���� ��ȭâ
		//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_MainTalk.json");
		//m_pCanvas_MainTalk = dynamic_cast<CCanvas_MainTalk*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_MainTalk", &json));
		//m_pCanvas_MainTalk->Add_Talk(0);
		//m_pCanvas_MainTalk->Add_Talk(1);
		//m_pCanvas_MainTalk->Add_Talk(2);

		//// ���� ��ȭâ
		//Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/Canvas_LeftTalk.json");
		//CCanvas_LeftTalk * pCanvas_MainTalk = dynamic_cast<CCanvas_LeftTalk * > (CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_Test", L"Canvas_LeftTalk", &json));
		//pCanvas_MainTalk->Add_Talk(0, true);
		//pCanvas_MainTalk->Add_Talk(1, true);
		//pCanvas_MainTalk->Add_Talk(2, true);
		//pCanvas_MainTalk->Add_Talk(3, true, 0);

		//m_pCanvas_SAGragting_Go->Set_Input();
		//m_pCanvas_SAContainer_Down->Set_Input();
	}

	 //Imgui_RenderProperty();
}

void CCanvas_MouseCousor::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	//static _float fHp = 1.0f;
	//static _float fMaxHp = 1.0f;
	//ImGui::SliderFloat("Hp", &fHp, 0.0f, 1.0f);
	//ImGui::SliderFloat("MaxHp", &fMaxHp, 0.0f, 1.0f);
	//m_pCanvas_BossHpMove->Set_BossHp(fHp / fMaxHp);

	static _int iExp = 0;
	ImGui::DragInt("Exp", &iExp);

	ImGui::Text("EXP %u", CPlayerInfoManager::GetInstance()->Get_PlayerStat().iExp);
	ImGui::Text("MAXEXP %u", CPlayerInfoManager::GetInstance()->Get_PlayerStat().iMaxExp);
	if (ImGui::Button("Level UP"))
	{
		CPlayerInfoManager::GetInstance()->Set_Exp(_uint(iExp));
	}

	if (ImGui::Button("Player Attack"))
	{
		CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_fBaseAttackDamage = 9000;
	}

	if (ImGui::Button("Add Etc Item"))
	{
		CItem_Manager::GetInstance()->Set_ItemCount(L"������ ����", 1);
		CItem_Manager::GetInstance()->Set_ItemCount(L"��ȭ�� ���� ���Ҵµ�.. �� ������", 1);
		CItem_Manager::GetInstance()->Set_ItemCount(L"�ʰ���� ���κ�", 1);
		CItem_Manager::GetInstance()->Set_ItemCount(L"������ ������", 1);
		CItem_Manager::GetInstance()->Set_ItemCount(L"�����̵��� ������", 1);
		CItem_Manager::GetInstance()->Set_ItemCount(L"������ ����ȯ", 1);
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