#include "stdafx.h"
#include "..\public\Canvas_BrainMap.h"
#include "GameInstance.h"
#include "PlayerInfoManager.h"
#include "UI_Manager.h"

#include "Canvas_PlayerInfoMove.h"
#include "DefaultUI.h"
#include "Main_PickUI.h"
#include "Main_OnMouseUI.h"
#include "Main_BrainMapIconPickUI.h"
#include "Main_BrainGaugeUI.h"

CCanvas_BrainMap::CCanvas_BrainMap(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_BrainMap::CCanvas_BrainMap(const CCanvas_BrainMap& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_BrainMap::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	m_iStartPriority = m_iPriority;

	return S_OK;
}

HRESULT CCanvas_BrainMap::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	m_bVisible = false;
	Brain_Intiialize();
	ChildUI_Intiialize();
	Link_Initialize();

	m_arrCurrentHighLevel.fill(0);

	return S_OK;
}

void CCanvas_BrainMap::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
	{
		if (true == m_bVisible)
		{
			// Lick �� m_bVisible �� �� ĵ������ true �� ���� �ǵ����� �ʴ´�.
			wstring	wbSLink = L"";
			if (10 > m_iLinkCount)
				wbSLink = L"Link__0" + to_wstring(m_iLinkCount);
			else
				wbSLink = L"Link__" + to_wstring(m_iLinkCount);

			if (iter->first == wbSLink)
			{
				++m_iLinkCount;
				if (56 == m_iLinkCount)
					m_iLinkCount = 0;
				continue;
			}

			wstring	wbSLinkS = L"Link__S_0" + to_wstring(m_iLinkSCount);
			if (iter->first == wbSLinkS)
			{
				++m_iLinkSCount;
				if (4 == m_iLinkSCount)
					m_iLinkSCount = 0;
				continue;
			}
		}

		iter->second->SetVisible(m_bVisible);
	}

	if (false == m_bVisible) return;
	OnIcon_Tick();
	SkillAcquisition_Tick();
	OnLick_Tick();
}

void CCanvas_BrainMap::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_BrainMap::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_tchar szText[MAX_PATH] = TEXT("");
	_float2 vPosition = { 0.0f, 0.0f };
	_float2 vFontBigSize = { 0.45f, 0.45f };
	_float2 vFontSmaillSize = { 0.35f, 0.35f };
	_float4 vColor = { 0.0f, 0.0f, 0.0f, 0.0f };

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (false == m_bSkillAcquisition)
	{
		m_iPriority = m_iStartPriority;
		vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
	}
	else
	{
		m_iPriority = 20;
		vColor = { 0.752f, 0.752f, 0.596f, 1.0f };

		vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"SkillAcquisition"))->GetScreenSpaceLeftTop();
		wsprintf(szText, TEXT("<%s>"), m_CurrentBrainInfo.szBrainName);
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(130.0f, 150.0f), 0.f, vFontSmaillSize * 1.5f, vColor);
		pGameInstance->Render_Font(L"Pretendard32", m_szAlarmText, vPosition + _float2(130.0f, 200.0f), 0.f, vFontSmaillSize * 1.5f, vColor);

		vColor = { 0.488f, 0.427f, 0.384f, 1.0f };
	}

	vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"BrainExplanation_Face"))->GetScreenSpaceLeftTop();
	wsprintf(szText, TEXT("%d"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iHP);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(385.0f, 10.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"ü��", vPosition + _float2(120.0f, 10.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().iSprbrPower);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(385.0f, 31.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"�ʳ��ɷ�", vPosition + _float2(120.0f, 31.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().iAttack);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(385.0f, 52.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"���ݷ�", vPosition + _float2(120.0f, 52.0f), 0.f, vFontSmaillSize, vColor);
	wsprintf(szText, TEXT("%d"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().iDefense);
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(385.0f, 75.0f), 0.f, vFontSmaillSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", L"����", vPosition + _float2(120.0f, 75.0f), 0.f, vFontSmaillSize, vColor);

	// ���콺 Ŀ�� �� �ö��� �� �Ǵ� Ŭ���� �Ǿ��� �� ������ ���� �Ѵ�.
	if (BRAINTYPE_END ==  m_CurrentBrainInfo.eType)
		return S_OK;

	vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"BrainExplanation"))->GetScreenSpaceLeftTop();

	pGameInstance->Render_Font(L"Pretendard32", m_CurrentBrainInfo.szBrainName, vPosition + _float2(100.0f, 120.0f), 0.f, { 0.6f, 0.6f }, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_CurrentBrainInfo.szBrainEx[0], vPosition + _float2(100.0f, 510.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_CurrentBrainInfo.szBrainEx[1], vPosition + _float2(100.0f, 540.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_CurrentBrainInfo.szBrainEx[2], vPosition + _float2(100.0f, 570.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_CurrentBrainInfo.szBrainEx[3], vPosition + _float2(100.0f, 600.0f), 0.f, vFontBigSize, vColor);

	if (true == m_CurrentBrainInfo.bUse)
	{
		pGameInstance->Render_Font(L"Pretendard32", L"���� �Ϸ�", vPosition + _float2(100.0f, 675.0f), 0.f, vFontBigSize, vColor);
	}
	else
	{
		// �� ���� BP
		wsprintf(szText, TEXT("/      %d BP"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().iBP);
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(141.0f, 675.0f), 0.f, vFontBigSize, vColor);

		if (false == m_bSkillAcquisition) // ���� BP�� ������ ���� ������ ������ ������ �������� ����Ѵ�.
		{
			// Icon �� BP
			if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().iBP >= m_vecIconUI[m_iCurrentIndex]->Get_BrainInfo().iBP)
				vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
			else
				vColor = { 0.488f, 0.427f, 0.384f, 1.0f };
		}

		wsprintf(szText, TEXT("%d"), m_CurrentBrainInfo.iBP);
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(112.0f, 675.0f), 0.f, vFontBigSize, vColor);
	}

	return S_OK;
}

void CCanvas_BrainMap::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();

}

void CCanvas_BrainMap::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_BrainMap::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_BrainMap::Brain_Intiialize()
{
	BRAININFO	tBrainInfo;
	tBrainInfo.arrNeighbor.fill(-1);

	// ����̺�
	// 0
	tBrainInfo.iBP = 1;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 3.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 2.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 1, 8, -1 };
	tBrainInfo.szBrainName = L"����̺� ������ ������ ���";
	tBrainInfo.szBrainEx[0] = L"����̺� ������ ������ +20%";
	tBrainInfo.szBrainEx[1] = L"���� �����߸��ų� ������ ������� �޴� ��";
	tBrainInfo.szBrainEx[2] = L"Ư�� �ൿ���� ���� �������� �����Ͽ�";
	tBrainInfo.szBrainEx[3] = L"����̺� �������� ���̱� ��������.";
	m_vecBrain.push_back(tBrainInfo);

	// 1
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.bUse = false;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 4, 2, -1 };
	tBrainInfo.szBrainName = L"����̺�: ü�� �ڵ� ȸ��";
	tBrainInfo.szBrainEx[0] = L"����̺� �� ü���� ������ ȸ���ȴ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 2
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 3, -1, -1 };
	tBrainInfo.szBrainName = L"����̺�: �����̻� ȸ��";
	tBrainInfo.szBrainEx[0] = L"����̺� �ߵ��ϸ� �����̻��� ȸ���Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 3
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"����̺�: ���� �Ʈ ����";
	tBrainInfo.szBrainEx[0] = L"����̺� �� <�Ʈ ����> ��";
	tBrainInfo.szBrainEx[1] = L"�������� �߻��ϰ� �ȴ�.";
	tBrainInfo.szBrainEx[2] = L"<�Ʈ ����>�� ���� ������ ����ϸ� �߻�";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 4
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 3.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 2.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 5, -1, -1 };
	tBrainInfo.szBrainName = L"����̺�: ����̺� ������ ȸ��1";
	tBrainInfo.szBrainEx[0] = L"����̺� �� �극��  ũ���÷� ���� �����߸���";
	tBrainInfo.szBrainEx[1] = L"����̺� �������� ���� ȸ���ȴ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 5
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 6, -1, -1 };
	tBrainInfo.szBrainName = L"����̺�: ���ʽ� ���� ������ ���";
	tBrainInfo.szBrainEx[0] = L"����̺� �� ȹ�� EXP�� ���ʽ� ������";
	tBrainInfo.szBrainEx[1] = L"�������� ����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 6
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 7, -1, -1 };
	tBrainInfo.szBrainName = L"����̺�: ���� ũ���� ȿ�� ���";
	tBrainInfo.szBrainEx[0] = L"����̺� �� �������� ũ���� ��������";
	tBrainInfo.szBrainEx[1] = L"���� ���� �� �ִ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 7
	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 3.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 2.0f, 3.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"����̺�: ����̺� ������ ȸ�� 2";
	tBrainInfo.szBrainEx[0] = L"����̺� �� �극�� ũ���÷� ���� �����߸���";
	tBrainInfo.szBrainEx[1] = L"����̺� �������� ���� ȸ���ȴ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 8
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 9, -1, -1 };
	tBrainInfo.szBrainName = L"����̺�: �������ͽ� ���";
	tBrainInfo.szBrainEx[0] = L"����̺� �� �ʳ��ɷ�, ���ݷ�, ���� +10%";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 9
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 10, -1, -1 };
	tBrainInfo.szBrainName = L"����̺�: SAS ������ ȸ��";
	tBrainInfo.szBrainEx[0] = L"����̺긦 �ߵ��ϸ�";
	tBrainInfo.szBrainEx[1] = L"��� SAS �������� ��� ȸ���ȴ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// ��ȭ
	// 10
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 11, 15, -1 };
	tBrainInfo.szBrainName = L"���� ������ ȸ���� ���(���� ����)";
	tBrainInfo.szBrainEx[0] = L"���� �������� ���� ���� �������� ȸ������ �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"!![�����̽���]+[���� ���罺] �÷����� ������ ���Ե��� �ʴ´�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 11
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 12, -1, -1 };
	tBrainInfo.szBrainName = L"���� ���� ũ���� ȿ�� ���2";
	tBrainInfo.szBrainEx[0] = L"���� �������� ũ���� �������� ���� ���� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"ũ���� ȿ���� 5% �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 12
	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 3.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 2.0f, 3.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"���� ������ �ִ�ġ ����";
	tBrainInfo.szBrainEx[0] = L"���� �̸��� ��ų�� 2�� ������, 1���� �����ϸ� 50%,";
	tBrainInfo.szBrainEx[1] = L"2���� �����ϸ� 100% ���� �������� �ִ�ġ�� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 13
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 5.0f };
	tBrainInfo.arrNeighbor = { 14, 17, 19 };
	tBrainInfo.szBrainName = L"���ݷ� ��� 1";
	tBrainInfo.szBrainEx[0] = L"���ݷ� +5%";
	tBrainInfo.szBrainEx[1] = L"���� �������� �ִ� ������� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 14
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 15, -1, -1 };
	tBrainInfo.szBrainName = L"�ļ�Ÿ";
	tBrainInfo.szBrainEx[0] = L"�ٿ� ���� ������ �ִ� ������� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 15
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 3.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 2.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 10, 16, -1 };
	tBrainInfo.szBrainName = L"�һ� �ӵ� ���";
	tBrainInfo.szBrainEx[0] = L"���Ḧ �һ��ϴ� �ӵ��� ����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 16
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 3.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"���� ���� ũ���� ȿ�� ��� 1";
	tBrainInfo.szBrainEx[0] = L"���� �������� ũ���� �������� ���� ���� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"ũ���� ȿ���� 5% �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 17
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 5.0f };
	tBrainInfo.arrNeighbor = { 18, -1, -1 };
	tBrainInfo.szBrainName = L"���ݷ� ��� 2";
	tBrainInfo.szBrainEx[0] = L"���ݷ� +5%";
	tBrainInfo.szBrainEx[1] = L"���� �������� �ִ� ������� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 18
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 3.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 2.0f, 3.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"���� ������ �ִ�ġ ����";
	tBrainInfo.szBrainEx[0] = L"���� �̸��� ��ų�� 2�� ������, 1���� �����ϸ� 50%,";
	tBrainInfo.szBrainEx[1] = L"2���� �����ϸ� 100% ���� �������� �ִ�ġ�� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 19
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 20, -1, -1 };
	tBrainInfo.szBrainName = L"�ʳ��ɷ� ���";
	tBrainInfo.szBrainEx[0] = L"�ʳ��ɷ� +5%";
	tBrainInfo.szBrainEx[1] = L"���� �������� �ִ� ������� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 20
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.bUse = false;
	tBrainInfo.arrNeighbor = { 21, -1, -1 };
	tBrainInfo.vOnIconIndex = { 2.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 3.0f };
	tBrainInfo.szBrainName = L"���� ���� ũ���� ȿ�� ���";
	tBrainInfo.szBrainEx[0] = L"���� �������� ũ���� �������� ���� ���� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"ũ���� ȿ���� 5% �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// Ȯ��
	// 21
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 5.0f };
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"����ġ�� ����";
	tBrainInfo.szBrainEx[0] = L"ü�� �߿� [C] ��� �������";
	tBrainInfo.szBrainEx[1] = L"���߿��� �ϰ��ϸ� ���� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 22
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 5.0f };
	tBrainInfo.arrNeighbor = { 21, 24, 30 };
	tBrainInfo.szBrainName = L"���� ����";
	tBrainInfo.szBrainEx[0] = L"[C] ��� ������� ���� ������, [C]�� ����";
	tBrainInfo.szBrainEx[1] = L"���� ������ ������ ���� ���� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 23

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 22, 31, -1 };
	tBrainInfo.szBrainName = L"����";
	tBrainInfo.szBrainEx[0] = L"������ �� [��]�� �ڼ��� �ٷ� ��� �ٿ��� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 24
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 3.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 2.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 25, -1, -1 };
	tBrainInfo.szBrainName = L"����Ʈ ȸ�� ���� ������ ȸ��";
	tBrainInfo.szBrainEx[0] = L"����Ʈ ȸ�� ���� �� ���� �������� ���� ȸ���Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 25
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 5.0f };
	tBrainInfo.arrNeighbor = { 26, -1, -1 };
	tBrainInfo.szBrainName = L"���� �޺� 4��";
	tBrainInfo.szBrainEx[0] = L"���� ������ 4ȸ���� �����ؼ� �� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 26
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 5.0f };
	tBrainInfo.arrNeighbor = { 27, 29, -1 };
	tBrainInfo.szBrainName = L"2�� ���� ����";
	tBrainInfo.szBrainEx[0] = L"[C] ��� ������� ���� ���� ��, �� �ܰ� �� ���� ������";
	tBrainInfo.szBrainEx[1] = L"[C] �� ���� <���� ����> �� ������ ��������. ";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 27
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 28, -1, -1 };
	tBrainInfo.szBrainName = L"���� ���� ��ȭ";
	tBrainInfo.szBrainEx[0] = L"<���� ����> �� <2�� ���� ����>��";
	tBrainInfo.szBrainEx[1] = L"���� ������ �о����� ������ ��������.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 28
	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 5.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"���� �ļ�Ÿ";
	tBrainInfo.szBrainEx[0] = L"�ٿ�� ������ [������ ���콺]�� ���� ������ �ϸ�";
	tBrainInfo.szBrainEx[1] = L"����ġ���� ���� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 29
	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 6;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 5.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"���� �޺� 5ź";
	tBrainInfo.szBrainEx[0] = L"���� ������ 5ȸ���� �����ؼ� �� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 30
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = GREEN;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 2.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"������ �������";
	tBrainInfo.szBrainEx[0] = L"��ó�� �������ִ� ��� �������� �ڵ����� �������.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 31
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 32, -1, -1 };
	tBrainInfo.szBrainName = L"����Ʈ ȸ�� ����";
	tBrainInfo.szBrainEx[0] = L"����Ʈ ȸ�� ���� �� [���� ���콺]�� ���� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 32
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 33, 37, -1 };
	tBrainInfo.szBrainName = L"���� ����";
	tBrainInfo.szBrainEx[0] = L"���� [�����̽���]�� �ѹ� �� ������ �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 33
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 1.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 34, -1, -1 };
	tBrainInfo.szBrainName = L"�߰� ���� Ȯ�� 1";
	tBrainInfo.szBrainEx[0] = L"���� �߰� ���ݰ� ���� �߰� ������";
	tBrainInfo.szBrainEx[1] = L"3ȸ���� �����ؼ� �� �� �ִ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 34
	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 5.0f };
	tBrainInfo.arrNeighbor = { 35, 36, -1 };
	tBrainInfo.szBrainName = L"ȸ�� ���� ���� Ȯ��";
	tBrainInfo.szBrainEx[0] = L"[C]�� �̵��ϸ鼭 ���� ������ �۾���";
	tBrainInfo.szBrainEx[1] = L"<ȸ�� ���� ����>��";
	tBrainInfo.szBrainEx[2] = L"�� �޺� �߿� 2������ �� �� �ִ�.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 35
	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 1.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 4.0f }; 
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"���� �߰� ����";
	tBrainInfo.szBrainEx[0] = L"���� ���� �޺� �߿� [������ ���콺]�� ���� �߰� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 36
	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 1.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 4.0f }; 
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"�߰� ���� Ȯ�� 2";
	tBrainInfo.szBrainEx[0] = L"���� �߰� ���ݰ� ���� �߰� ������";
	tBrainInfo.szBrainEx[1] = L"4ȸ���� �����ؼ� �� �� �ִ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 37
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 3.0f }; 
	tBrainInfo.arrNeighbor = { 38, 41, -1 };
	tBrainInfo.szBrainName = L"���� ����";
	tBrainInfo.szBrainEx[0] = L"���� [��]�� ���� ������ �� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 38
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 2.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 1.0f, 5.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"���� ��� ����";
	tBrainInfo.szBrainEx[0] = L"<���� ����> �� [���� ���콺]�� �����ϸ鼭 ���� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// ����
	// 39
	tBrainInfo.iBP = 1;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 40, 46, -1 };
	tBrainInfo.szBrainName = L"�÷����� ��� ���� �߰� 1";
	tBrainInfo.szBrainEx[0] = L"��� ȭ�鿡�� ������ �� �ִ� �÷������� �ϳ� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 40
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = GREEN;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 2.0f };
	tBrainInfo.arrNeighbor = { 41, -1, -1 };
	tBrainInfo.szBrainName = L"������ ��� ���� ����";
	tBrainInfo.szBrainEx[0] = L"������ ��� ���� ��� ���� �ð��� �����Ų��.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 41
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 1.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 37, 42, -1 };
	tBrainInfo.szBrainName = L"�극�� ũ���� SAS ȸ��";
	tBrainInfo.szBrainEx[0] = L"SAS ȸ�� �߿� �극�� ũ���÷� ���� �����߸���";
	tBrainInfo.szBrainEx[1] = L"��� ���� SAS�� SAS �������� ���� ȸ���Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 42
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 1.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 43, 44, -1 };
	tBrainInfo.szBrainName = L"�극�� ũ���� ȹ�� �ݾ� ���ʽ�";
	tBrainInfo.szBrainEx[0] = L"�극�� ũ���÷� ���� �����߸���";
	tBrainInfo.szBrainEx[1] = L"���� �����߷��� �� �Լ��� �� �ִ� ���� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 43
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 1.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 3.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"�극�� ũ���� ȹ�� EXP ���ʽ�";
	tBrainInfo.szBrainEx[0] = L"�극�� ũ���÷� ���� �����߸��� ȹ�� EXP�� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 44
	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 45, -1, -1 };
	tBrainInfo.szBrainName = L"�÷����� ��� ���� �߰� 2";
	tBrainInfo.szBrainEx[0] = L"��� ȭ�鿡�� ������ �� �ִ� �÷������� �ϳ� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 45
	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"SAS ���� �ߵ�";
	tBrainInfo.szBrainEx[0] = L"���� �̸��� ��ų�� 2�� ������,";
	tBrainInfo.szBrainEx[1] = L"�ϳ��� �����ϸ�, SAS�� ���ÿ� 2������ �ߵ��� �� ������,";
	tBrainInfo.szBrainEx[2] = L"2���� �����ϸ�, SAS�� ���ÿ� 4������ �ߵ��� �� �ִ�.";
	tBrainInfo.szBrainEx[3] = L"!!SAS ��ȭ�� SAS ������ ���ÿ� �ߵ��� �� ����.";
	m_vecBrain.push_back(tBrainInfo);

	// 46
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 47, 48, -1 };
	tBrainInfo.szBrainName = L"���� ų";
	tBrainInfo.szBrainEx[0] = L"ũ���� �������� �� �￩ Ȱ���� ������";
	tBrainInfo.szBrainEx[1] = L"������ ������ ���߽�Ű�� EXP ���ʽ��� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"��, �극�� ũ���÷ιۿ� �����߸� �� ���� ������ ���,";
	tBrainInfo.szBrainEx[3] = L"ü���� �� ���� ��Ȳ������ ȿ���� ����.";
	m_vecBrain.push_back(tBrainInfo);

	// 47
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 1.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 3.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"�극�� ũ���� ���� ����";
	tBrainInfo.szBrainEx[0] = L"���� ũ���� �������� �� ���� ��";
	tBrainInfo.szBrainEx[1] = L"���� �� ���� �ð� Ȱ���� �����ϰ� �Ǿ�";
	tBrainInfo.szBrainEx[2] = L"�극�� ũ���� ������ �ð��� �����Ѵ�.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 48
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"SAS ���� �ߵ�";
	tBrainInfo.szBrainEx[0] = L"���� �̸��� ��ų�� 2�� ������,";
	tBrainInfo.szBrainEx[1] = L"�ϳ��� �����ϸ�, SAS�� ���ÿ� 2������ �ߵ��� �� ������,";
	tBrainInfo.szBrainEx[2] = L"2���� �����ϸ�, SAS�� ���ÿ� 4������ �ߵ��� �� �ִ�.";
	tBrainInfo.szBrainEx[3] = L"!!SAS ��ȭ�� SAS ������ ���ÿ� �ߵ��� �� ����.";
	m_vecBrain.push_back(tBrainInfo);
	// ��� �ٸ� ��

	// �극�� �ʵ�
	// 49
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = BRAINFIELD;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 3.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 2.0f, 3.0f };
	tBrainInfo.arrNeighbor = { 50, 54, 58 };
	tBrainInfo.szBrainName = L"�극�� �ʵ�: �ð� ���� 1";
	tBrainInfo.szBrainEx[0] = L"�극�� �ʵ��� ���� �ð��� 5�� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 50
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = BRAINFIELD;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 51, 53, -1 };
	tBrainInfo.szBrainName = L"�극�� �ʵ�: ���� ���� ���� ���";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� ��";
	tBrainInfo.szBrainEx[1] = L"[G] ��� ������� ���̴� ������Ʈ�� �����Ͽ�";
	tBrainInfo.szBrainEx[2] = L"������Ʈ�� ���Ͻ��Ѽ� �ִ� ������� �����Ѵ�.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// ���� _2
	// 51
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = GREEN;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 0.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 2.0f };
	tBrainInfo.arrNeighbor = { 48, 52, -1 };
	tBrainInfo.szBrainName = L"������ ��� ȿ��";
	tBrainInfo.szBrainEx[0] = L"������ ��� ���� ������ ������";
	tBrainInfo.szBrainEx[1] = L"��� ȿ���� ���� �� �ִ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 52
	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"ü�� �ڵ� ȸ��";
	tBrainInfo.szBrainEx[0] = L"������ �� ü���� ������ ȸ���ȴ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// �ٽ� �극�� �ʵ�
	// 53
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"�극�� �ʵ�: ����� �氨";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� �� �޴� ��������";
	tBrainInfo.szBrainEx[1] = L"�氨�ȴ�. ���� �̸��� ��ų�� 2�� ������";
	tBrainInfo.szBrainEx[2] = L"1�� �����ϸ� ������� �ݰ�,";
	tBrainInfo.szBrainEx[3] = L"2�� �����ϸ� ����ڰ� ��ȿȭ�ȴ�.";
	m_vecBrain.push_back(tBrainInfo);

	// 54
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 55, -1, -1 };
	tBrainInfo.szBrainName = L"�극�� �ʵ�: ���� ų";
	tBrainInfo.szBrainEx[0] = L"�극�� �ʵ� ���� ��, ũ���� �������� �� ����";
	tBrainInfo.szBrainEx[1] = L"������ ������ ���߽�Ű�� EXP ���ʽ��� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"��' �극�� ũ���÷ιۿ� �����߸� �� ���� ������ ���";
	tBrainInfo.szBrainEx[3] = L"ü���� �� ���� ��Ȳ������ ȿ���� ����.";
	m_vecBrain.push_back(tBrainInfo);

	// 55
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 56, 57, -1 };
	tBrainInfo.szBrainName = L"�극�� �ʵ�: Ÿ�� ���� ���";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� ��";
	tBrainInfo.szBrainEx[1] = L"[���� ���콺]�� �ֵθ��� ������Ʈ�� �����Ͽ�";
	tBrainInfo.szBrainEx[2] = L"������� �� �� �ִ� ������ �о�����.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 56
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"�극�� �ʵ�: ����̺� �� ���� �Һ� ����";
	tBrainInfo.szBrainEx[0] = L"�극�� �ʵ��� �غ� �������� ǥ�õ� ���¿���";
	tBrainInfo.szBrainEx[1] = L"����̺긦 �ߵ����� ���, ����̺� �� ������";
	tBrainInfo.szBrainEx[2] = L"�Һ����� �ʰ� �극�� �ʵ带 ������ �� �ִ�.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 57
	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 1.0f,  1.0f };
	tBrainInfo.vOffIconIndex = { 0.0f, 4.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"�극�� �ʵ�: ����� �氨";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� �� �޴� ��������";
	tBrainInfo.szBrainEx[1] = L"�氨�ȴ�. ���� �̸��� ��ų�� 2�� ������";
	tBrainInfo.szBrainEx[2] = L"1�� �����ϸ� ������� �ݰ�,";
	tBrainInfo.szBrainEx[3] = L"2�� �����ϸ� ����ڰ� ��ȿȭ�ȴ�.";
	m_vecBrain.push_back(tBrainInfo);

	// 58
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = BRAINFIELD;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 59, -1, -1 };
	tBrainInfo.szBrainName = L"�극�� �ʵ�: �߻� ���� ��� 1";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� ��,";
	tBrainInfo.szBrainEx[1] = L"[���� ���콺 2] [������ ���콺] �� ";
	tBrainInfo.szBrainEx[2] = L"[���� ���콺 3] [������ ���콺]�� [������ ���콺]��";
	tBrainInfo.szBrainEx[3] = L"������ ������Ʈ�� �����Ѵ�.";
	m_vecBrain.push_back(tBrainInfo);

	// 59
	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 4.0f };
	tBrainInfo.arrNeighbor = { 60, -1, -1 };
	tBrainInfo.szBrainName = L"�극�� �ʵ�: �߻� ���� ��� 2";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� ��";
	tBrainInfo.szBrainEx[1] = L"[���� ���콺 3] [������ ���콺]��";
	tBrainInfo.szBrainEx[2] = L"[������ ���콺]�� ������ ������Ʈ�� ���� �����Ѵ�.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// 60
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.bUse = false;
	tBrainInfo.vOnIconIndex = { 0.0f, 2.0f };
	tBrainInfo.vOffIconIndex = { 3.0f, 4.0f };
	tBrainInfo.arrNeighbor = { -1, -1, -1 };
	tBrainInfo.szBrainName = L"�극�� �ʵ�: �ð� ���� 2";
	tBrainInfo.szBrainEx[0] = L"�극�� �ʵ� ���� �ð��� 5�� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);
}

void CCanvas_BrainMap::ChildUI_Intiialize()
{
	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
	{
		iter->second->SetVisible(false);

		wstring	wsIcon = L"";
		if (10 > m_iIconCount)
			wsIcon = L"Icon__0" + to_wstring(m_iIconCount);
		else
			wsIcon = L"Icon__" + to_wstring(m_iIconCount);

		if (iter->first == wsIcon)
		{
			CMain_OnMouseUI* pMouseUI = dynamic_cast<CMain_OnMouseUI*>((*iter).second);
			pMouseUI->Set_BrainInfo(m_vecBrain[m_iIconCount]);
			
			if(0 == m_vecBrain[m_iIconCount].iLevel)
				pMouseUI->Set_IconIndex(m_vecBrain[m_iIconCount].vOnIconIndex);
			else
				pMouseUI->Set_IconIndex(m_vecBrain[m_iIconCount].vOffIconIndex);

			m_vecIconUI.push_back(pMouseUI);

			++m_iIconCount;
		}
	}
}

void CCanvas_BrainMap::Link_Initialize()
{
	LINKINFO	tLinkInfo;

	tLinkInfo.bSLink = false;

	tLinkInfo.iIconIndex0 = 0;
	tLinkInfo.iIconIndex1 = 1;
	tLinkInfo.iLickIndex = 0;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 1;
	tLinkInfo.iIconIndex1 = 4;
	tLinkInfo.iLickIndex = 1;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 4;
	tLinkInfo.iIconIndex1 = 5;
	tLinkInfo.iLickIndex = 2;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 5;
	tLinkInfo.iIconIndex1 = 6;
	tLinkInfo.iLickIndex = 3;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 6;
	tLinkInfo.iIconIndex1 = 7;
	tLinkInfo.iLickIndex = 4;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 6;
	tLinkInfo.iIconIndex1 = 7;
	tLinkInfo.iLickIndex = 4;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 1;
	tLinkInfo.iIconIndex1 = 2;
	tLinkInfo.iLickIndex = 5;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 2;
	tLinkInfo.iIconIndex1 = 3;
	tLinkInfo.iLickIndex = 6;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 59;
	tLinkInfo.iIconIndex1 = 60;
	tLinkInfo.iLickIndex = 7;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 8;
	tLinkInfo.iIconIndex1 = 9;
	tLinkInfo.iLickIndex = 8;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 10;
	tLinkInfo.iIconIndex1 = 11;
	tLinkInfo.iLickIndex = 9;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 11;
	tLinkInfo.iIconIndex1 = 12;
	tLinkInfo.iLickIndex = 10;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 8;
	tLinkInfo.iIconIndex1 = 0;
	tLinkInfo.iLickIndex = 11;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 10;
	tLinkInfo.iIconIndex1 = 15;
	tLinkInfo.iLickIndex = 12;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 13;
	tLinkInfo.iIconIndex1 = 14;
	tLinkInfo.iLickIndex = 13;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 15;
	tLinkInfo.iIconIndex1 = 14;
	tLinkInfo.iLickIndex = 14;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 16;
	tLinkInfo.iIconIndex1 = 15;
	tLinkInfo.iLickIndex = 15;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 13;
	tLinkInfo.iIconIndex1 = 17;
	tLinkInfo.iLickIndex = 16;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 18;
	tLinkInfo.iIconIndex1 = 17;
	tLinkInfo.iLickIndex = 17;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 13;
	tLinkInfo.iIconIndex1 = 19;
	tLinkInfo.iLickIndex = 18;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 19;
	tLinkInfo.iIconIndex1 = 20;
	tLinkInfo.iLickIndex = 19;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 21;
	tLinkInfo.iIconIndex1 = 22;
	tLinkInfo.iLickIndex = 20;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 22;
	tLinkInfo.iIconIndex1 = 24;
	tLinkInfo.iLickIndex = 21;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 24;
	tLinkInfo.iIconIndex1 = 25;
	tLinkInfo.iLickIndex = 22;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 25;
	tLinkInfo.iIconIndex1 = 26;
	tLinkInfo.iLickIndex = 23;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 26;
	tLinkInfo.iIconIndex1 = 27;
	tLinkInfo.iLickIndex = 24;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 27;
	tLinkInfo.iIconIndex1 = 28;
	tLinkInfo.iLickIndex = 25;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 26;
	tLinkInfo.iIconIndex1 = 29;
	tLinkInfo.iLickIndex = 26;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 22;
	tLinkInfo.iIconIndex1 = 23;
	tLinkInfo.iLickIndex = 27;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 22;
	tLinkInfo.iIconIndex1 = 30;
	tLinkInfo.iLickIndex = 28;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 23;
	tLinkInfo.iIconIndex1 = 31;
	tLinkInfo.iLickIndex = 29;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 31;
	tLinkInfo.iIconIndex1 = 32;
	tLinkInfo.iLickIndex = 30;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 32;
	tLinkInfo.iIconIndex1 = 33;
	tLinkInfo.iLickIndex = 31;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 33;
	tLinkInfo.iIconIndex1 = 34;
	tLinkInfo.iLickIndex = 32;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 35;
	tLinkInfo.iIconIndex1 = 34;
	tLinkInfo.iLickIndex = 33;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 34;
	tLinkInfo.iIconIndex1 = 36;
	tLinkInfo.iLickIndex = 34;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 32;
	tLinkInfo.iIconIndex1 = 37;
	tLinkInfo.iLickIndex = 35;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 37;
	tLinkInfo.iIconIndex1 = 38;
	tLinkInfo.iLickIndex = 36;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 39;
	tLinkInfo.iIconIndex1 = 40;
	tLinkInfo.iLickIndex = 37;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 40;
	tLinkInfo.iIconIndex1 = 41;
	tLinkInfo.iLickIndex = 38;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 41;
	tLinkInfo.iIconIndex1 = 42;
	tLinkInfo.iLickIndex = 39;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 42;
	tLinkInfo.iIconIndex1 = 43;
	tLinkInfo.iLickIndex = 40;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 42;
	tLinkInfo.iIconIndex1 = 44;
	tLinkInfo.iLickIndex = 41;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 44;
	tLinkInfo.iIconIndex1 = 45;
	tLinkInfo.iLickIndex = 42;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 39;
	tLinkInfo.iIconIndex1 = 46;
	tLinkInfo.iLickIndex = 43;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 46;
	tLinkInfo.iIconIndex1 = 48;
	tLinkInfo.iLickIndex = 44;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 46;
	tLinkInfo.iIconIndex1 = 47;
	tLinkInfo.iLickIndex = 45;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 51;
	tLinkInfo.iIconIndex1 = 48;
	tLinkInfo.iLickIndex = 46;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 51;
	tLinkInfo.iIconIndex1 = 52;
	tLinkInfo.iLickIndex = 47;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 49;
	tLinkInfo.iIconIndex1 = 50;
	tLinkInfo.iLickIndex = 48;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 50;
	tLinkInfo.iIconIndex1 = 53;
	tLinkInfo.iLickIndex = 49;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 49;
	tLinkInfo.iIconIndex1 = 54;
	tLinkInfo.iLickIndex = 50;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 54;
	tLinkInfo.iIconIndex1 = 55;
	tLinkInfo.iLickIndex = 51;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 55;
	tLinkInfo.iIconIndex1 = 56;
	tLinkInfo.iLickIndex = 52;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 55;
	tLinkInfo.iIconIndex1 = 57;
	tLinkInfo.iLickIndex = 53;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 49;
	tLinkInfo.iIconIndex1 = 58;
	tLinkInfo.iLickIndex = 54;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 58;
	tLinkInfo.iIconIndex1 = 59;
	tLinkInfo.iLickIndex = 55;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	// S �� ��� Link
	tLinkInfo.bSLink = true;

	tLinkInfo.iIconIndex0 = 9;
	tLinkInfo.iIconIndex1 = 10;
	tLinkInfo.iLickIndex = 0;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 20;
	tLinkInfo.iIconIndex1 = 21;
	tLinkInfo.iLickIndex = 1;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 37;
	tLinkInfo.iIconIndex1 = 41;
	tLinkInfo.iLickIndex = 2;
	m_vecOnLinkInfo.push_back(tLinkInfo);

	tLinkInfo.iIconIndex0 = 50;
	tLinkInfo.iIconIndex1 = 51;
	tLinkInfo.iLickIndex = 3;
	m_vecOnLinkInfo.push_back(tLinkInfo);

}

void CCanvas_BrainMap::OnIcon_Tick()
{
	// �ڽ� UI�� ���鼭 ���ؾ� �� �� �� Ȯ���ؾ� �Ѵ�.
	for (size_t i = 0; i < m_vecIconUI.size(); ++i)
	{
		if (m_bSkillAcquisition == false)
		{
			// ������ ���� ���콺�� �ö��� ��
			if (true == m_vecIconUI[i]->Get_OnMouse())
			{
				m_iCurrentIndex = i;
				Find_ChildUI(L"Icon_Pick")->Set_Position(m_vecIconUI[i]->Get_Position());
				m_CurrentBrainInfo = m_vecIconUI[i]->Get_BrainInfo();
			}
		}

		if (false == m_vecIconUI[i]->Get_BrainInfo().bUse)
		{
			// �������� Ŭ�� ���� ��
			if (true == m_vecIconUI[i]->Get_OnButton())
			{
				IconPick(i);
				m_iCurrentIndex = i;
				m_vecIconUI[i]->Set_OnButton();
				dynamic_cast<CMain_BrainMapIconPickUI*>(Find_ChildUI(L"Icon_Pick"))->Set_Pick();
			}
		}
	}
}

void CCanvas_BrainMap::IconPick(const size_t iIndex)
{
	// ���� ������ �� �ִ� Level �� �Ǿ�� �� �� �ֵ��� �Ѵ�.
	if (m_vecBrain[iIndex].vOnIconIndex != m_vecIconUI[iIndex]->Get_CurrentIconIndex())
	{
		m_bSkillAcquisition = true;
		m_szAlarmText = L"���� ������ �������� �ʽ��ϴ�.";
		return;
	}

	// ���� �÷��̾ ������ �ִ� BP�� Ȯ���ؼ� Icon �� BP ���� ū ��쿡�� ������ �� �ִ�.
	if (CPlayerInfoManager::GetInstance()->Get_PlayerStat().iBP >= m_vecIconUI[iIndex]->Get_BrainInfo().iBP)
	{			
		// �������� ���� ���� �������� ������ �� �ִ�.
		dynamic_cast<CMain_BrainGaugeUI*>(Find_ChildUI(L"IconGauge_0"))->Set_ChargeGauge(true);
		dynamic_cast<CMain_BrainGaugeUI*>(Find_ChildUI(L"IconGauge_1"))->Set_ChargeGauge(true);
		Find_ChildUI(L"IconGauge_0")->Set_Position(m_vecIconUI[iIndex]->Get_Position());
		Find_ChildUI(L"IconGauge_1")->Set_Position(m_vecIconUI[iIndex]->Get_Position());

		if (true == dynamic_cast<CMain_BrainGaugeUI*>(Find_ChildUI(L"IconGauge_0"))->Get_GaugeFull())
		{
			m_bSkillAcquisition = true;
			m_szAlarmText = L"BP�� �Ҹ��߽��ϴ�.";
			m_vecIconUI[iIndex]->Set_BrainUse();

			_uint iResultBP = CPlayerInfoManager::GetInstance()->Get_PlayerStat().iBP - m_vecIconUI[iIndex]->Get_BrainInfo().iBP;
			CPlayerInfoManager::GetInstance()->Set_BP(iResultBP);	 // �÷��̾� BP �����ϱ�

			if (iIndex == 12 || iIndex == 18) 	// �÷��̾� ���� ���� ����
			{
				CPlayerInfoManager::GetInstance()->Get_PlayerStat().m_iKineticEnergyLevel += 1;
				dynamic_cast<CCanvas_PlayerInfoMove*>(CUI_Manager::GetInstance()->Find_MoveCanvas(L"Canvas_PlayerInfoMove"))->Set_PsychokinesisType();
			}

			// ������ ���� ������ ������ �� �ִ� ����
			for (_int j = 0; j < 3; ++j)
			{
				if (-1 == m_vecIconUI[iIndex]->Get_BrainInfo().arrNeighbor[j])
					break;

				m_vecIconUI[m_vecIconUI[iIndex]->Get_BrainInfo().arrNeighbor[j]]->Set_OnIcon();
			}
			return;
		}
	}
	else
	{
		if (true == m_vecIconUI[iIndex]->Get_BrainInfo().bUse)
			return;

		m_bSkillAcquisition = true;
		m_szAlarmText = L"BP�� �����մϴ�.";
		return;
	}
}

void CCanvas_BrainMap::SkillAcquisition_Tick()
{
	Find_ChildUI(L"SkillAcquisition")->SetVisible(m_bSkillAcquisition);
	Find_ChildUI(L"SkillAcquisitionBorderLine")->SetVisible(m_bSkillAcquisition);
	Find_ChildUI(L"SkillAcquisitionBorderLine_B")->SetVisible(m_bSkillAcquisition);
	Find_ChildUI(L"SkillAcquisition_BackGround")->SetVisible(m_bSkillAcquisition);
	Find_ChildUI(L"SkillAcquisition_Button")->SetVisible(m_bSkillAcquisition);
	dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"SkillAcquisition_Button"))->Set_ColorType(2);

	if (false == m_bSkillAcquisition)
		return;

	if (true == dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"SkillAcquisition_Button"))->Get_OnButton() ||
		CGameInstance::GetInstance()->KeyDown(DIK_RETURN))
	{
		m_bSkillAcquisition = false;
		dynamic_cast<CMain_PickUI*>(Find_ChildUI(L"SkillAcquisition_Button"))->Set_OnButton();

		dynamic_cast<CMain_BrainGaugeUI*>(Find_ChildUI(L"IconGauge_0"))->Set_ChargeGauge(false);
		dynamic_cast<CMain_BrainGaugeUI*>(Find_ChildUI(L"IconGauge_1"))->Set_ChargeGauge(false);
		return;
	}
}

void CCanvas_BrainMap::OnLick_Tick()
{
	for (size_t i = 0; i < m_vecOnLinkInfo.size(); ++i)
	{
		size_t iIconIndex0 = m_vecOnLinkInfo[i].iIconIndex0;
		size_t iIconIndex1 = m_vecOnLinkInfo[i].iIconIndex1;

		_bool	bIconUse0 = m_vecIconUI[iIconIndex0]->Get_BrainInfo().bUse;
		_bool	bIconUse1 = m_vecIconUI[iIconIndex1]->Get_BrainInfo().bUse;

		if (true == bIconUse0 && true == bIconUse1)
		{
			_tchar szText[MAX_PATH] = TEXT("");
			if (true == m_vecOnLinkInfo[i].bSLink)
			{
				wsprintf(szText, TEXT("Link__S_0%d"), static_cast<_int>(m_vecOnLinkInfo[i].iLickIndex));
			}
			else
			{
				if (10 > m_vecOnLinkInfo[i].iLickIndex)
					wsprintf(szText, TEXT("Link__0%d"), static_cast<_int>(m_vecOnLinkInfo[i].iLickIndex));
				else
					wsprintf(szText, TEXT("Link__%d"), static_cast<_int>(m_vecOnLinkInfo[i].iLickIndex));
			}
			Find_ChildUI(szText)->SetVisible(true);
		}
	}
}

CCanvas_BrainMap * CCanvas_BrainMap::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_BrainMap*		pInstance = new CCanvas_BrainMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_BrainMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_BrainMap::Clone(void * pArg)
{
	CCanvas_BrainMap*		pInstance = new CCanvas_BrainMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_BrainMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_BrainMap::Free()
{
	CCanvas::Free();

	m_vecBrain.clear();
	m_vecIconUI.clear();
	m_vecOnLinkInfo.clear();
}
