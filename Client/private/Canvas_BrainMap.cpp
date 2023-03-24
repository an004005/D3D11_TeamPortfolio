#include "stdafx.h"
#include "..\public\Canvas_BrainMap.h"
#include "GameInstance.h"
#include "PlayerInfoManager.h"

#include "DefaultUI.h"
#include "Main_OnMouseUI.h"
#include "Main_BrainMapIconPickUI.h"

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

	return S_OK;
}

HRESULT CCanvas_BrainMap::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	Brain_Intiialize();

	m_bVisible = false;

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
			m_vecIconUI.push_back(pMouseUI);

			++m_iIconCount;
		}

		wstring	wsLink = L"";
		if (10 > m_iLinkCount)
			wsLink = L"Icon__0" + to_wstring(m_iLinkCount);
		else
			wsLink = L"Icon__" + to_wstring(m_iLinkCount);

		if (iter->first == wsLink)
		{
			++m_iLinkCount;
			m_vecLinkUI.push_back(iter->second);
		}
	}

	return S_OK;
}

void CCanvas_BrainMap::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(m_bVisible);

	OnIcon_Tick();

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
	_float2 vFontBigSize = { 0.45f, 0.45f };
	_float2 vFontSmaillSize = { 0.35f, 0.35f };
	_float4 vColor = { 0.752f, 0.752f, 0.596f, 1.0f };
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_float2 vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"BrainExplanation_Face"))->GetScreenSpaceLeftTop();
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
	if (BRAINTYPE_END ==  m_BrainInfo.eType)
		return S_OK;

	vPosition = dynamic_cast<CDefaultUI*>(Find_ChildUI(L"BrainExplanation"))->GetScreenSpaceLeftTop();

	pGameInstance->Render_Font(L"Pretendard32", m_BrainInfo.szBrainName, vPosition + _float2(100.0f, 120.0f), 0.f, { 0.6f, 0.6f }, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_BrainInfo.szBrainEx[0], vPosition + _float2(100.0f, 510.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_BrainInfo.szBrainEx[1], vPosition + _float2(100.0f, 540.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_BrainInfo.szBrainEx[2], vPosition + _float2(100.0f, 570.0f), 0.f, vFontBigSize, vColor);
	pGameInstance->Render_Font(L"Pretendard32", m_BrainInfo.szBrainEx[3], vPosition + _float2(100.0f, 600.0f), 0.f, vFontBigSize, vColor);

	if (true == m_bAcquired)
	{
		pGameInstance->Render_Font(L"Pretendard32", L"���� �Ϸ�", vPosition + _float2(80.0f, 21.0f), 0.f, vFontBigSize, vColor);
	}
	else
	{
		// �� ���� BP
		wsprintf(szText, TEXT("/      %d BP"), CPlayerInfoManager::GetInstance()->Get_PlayerStat().iBP);
		pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(141.0f, 675.0f), 0.f, vFontBigSize, vColor);

		// UITODO : �� �������� BP �� ����־�� �Ѵ�. + �� BP�� �����ϴٸ� vColor �� ������ �������ش�.
		wsprintf(szText, TEXT("%d"), 55);
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

	// ����̺�
	tBrainInfo.iBP = 1;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺� ������ ������ ���";
	tBrainInfo.szBrainEx[0] = L"����̺� ������ ������ +20%";
	tBrainInfo.szBrainEx[1] = L"���� �����߸��ų� ������ ������� �޴� ��";
	tBrainInfo.szBrainEx[2] = L"Ư�� �ൿ���� ���� �������� �����Ͽ�";
	tBrainInfo.szBrainEx[3] = L"����̺� �������� ���̱� ��������.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺�: ü�� �ڵ� ȸ��";
	tBrainInfo.szBrainEx[0] = L"����̺� �� ü���� ������ ȸ���ȴ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺�: �����̻� ȸ��";
	tBrainInfo.szBrainEx[0] = L"����̺� �ߵ��ϸ� �����̻��� ȸ���Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺�: ���� �Ʈ ����";
	tBrainInfo.szBrainEx[0] = L"����̺� �� ���Ʈ ������ ��";
	tBrainInfo.szBrainEx[1] = L"�������� �߻��ϰ� �ȴ�.";
	tBrainInfo.szBrainEx[2] = L"�ء��Ʈ �������� ���� ������ ����ϸ� �߻�";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺�: ����̺� ������ ȸ��1";
	tBrainInfo.szBrainEx[0] = L"����̺� �� �극��  ũ���÷� ���� �����߸���";
	tBrainInfo.szBrainEx[1] = L"����̺� �������� ���� ȸ���ȴ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺�: ���ʽ� ���� ������ ���";
	tBrainInfo.szBrainEx[0] = L"����̺� �� ȹ�� EXP�� ���ʽ� ������";
	tBrainInfo.szBrainEx[1] = L"�������� ����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"����̺�: ���� ũ���� ȿ�� ���";
	tBrainInfo.szBrainEx[0] = L"����̺� �� �������� ũ���� ��������";
	tBrainInfo.szBrainEx[1] = L"���� ���� �� �ִ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺�: ����̺� ������ ȸ�� 2";
	tBrainInfo.szBrainEx[0] = L"����̺� �� �극�� ũ���÷� ���� �����߸���";
	tBrainInfo.szBrainEx[1] = L"����̺� �������� ���� ȸ���ȴ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺�: �������ͽ� ���";
	tBrainInfo.szBrainEx[0] = L"����̺� �� �ʳ��ɷ�, ���ݷ�, ���� +10%";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = DRIVE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����̺�: SAS ������ ȸ��";
	tBrainInfo.szBrainEx[0] = L"����̺긦 �ߵ��ϸ�";
	tBrainInfo.szBrainEx[1] = L"��� SAS �������� ��� ȸ���ȴ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// ��ȭ
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ������ ȸ���� ���(���� ����)";
	tBrainInfo.szBrainEx[0] = L"���� �������� ���� ���� �������� ȸ������ �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"��[�����̽���]+[���� ���罺] �÷����� ������ ���Ե��� �ʴ´�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"���� ���� ũ���� ȿ�� ���2";
	tBrainInfo.szBrainEx[0] = L"���� �������� ũ���� �������� ���� ���� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"ũ���� ȿ���� 5% �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ������ �ִ�ġ ����";
	tBrainInfo.szBrainEx[0] = L"���� �̸��� ��ų�� 2�� ������, 1���� �����ϸ� 50%,";
	tBrainInfo.szBrainEx[1] = L"2���� �����ϸ� 100% ���� �������� �ִ�ġ�� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���ݷ� ��� 1";
	tBrainInfo.szBrainEx[0] = L"���ݷ� +5%";
	tBrainInfo.szBrainEx[1] = L"���� �������� �ִ� ������� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�ļ�Ÿ";
	tBrainInfo.szBrainEx[0] = L"�ٿ� ���� ������ �ִ� ������� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�һ� �ӵ� ���";
	tBrainInfo.szBrainEx[0] = L"���Ḧ �һ��ϴ� �ӵ��� ����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ���� ũ���� ȿ�� ��� 1";
	tBrainInfo.szBrainEx[0] = L"���� �������� ũ���� �������� ���� ���� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"ũ���� ȿ���� 5% �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���ݷ� ��� 2";
	tBrainInfo.szBrainEx[0] = L"���ݷ� +5%";
	tBrainInfo.szBrainEx[1] = L"���� �������� �ִ� ������� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ������ �ִ�ġ ����";
	tBrainInfo.szBrainEx[0] = L"���� �̸��� ��ų�� 2�� ������, 1���� �����ϸ� 50%,";
	tBrainInfo.szBrainEx[1] = L"2���� �����ϸ� 100% ���� �������� �ִ�ġ�� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"�ʳ��ɷ� ���";
	tBrainInfo.szBrainEx[0] = L"�ʳ��ɷ� +5%";
	tBrainInfo.szBrainEx[1] = L"���� �������� �ִ� ������� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ENHANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"���� ���� ũ���� ȿ�� ���";
	tBrainInfo.szBrainEx[0] = L"���� �������� ũ���� �������� ���� ���� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"ũ���� ȿ���� 5% �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// Ȯ��
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����ġ�� ����";
	tBrainInfo.szBrainEx[0] = L"ü�� �߿� [C] ��� �������";
	tBrainInfo.szBrainEx[1] = L"���߿��� �ϰ��ϸ� ���� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ����";
	tBrainInfo.szBrainEx[0] = L"[C] ��� ������� ���� ������, [C]�� ����";
	tBrainInfo.szBrainEx[1] = L"���� ������ ������ ���� ���� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����";
	tBrainInfo.szBrainEx[0] = L"������ �� [��]�� �ڼ��� �ٷ� ��� �ٿ��� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����Ʈ ȸ�� ���� ������ ȸ��";
	tBrainInfo.szBrainEx[0] = L"����Ʈ ȸ�� ���� �� ���� �������� ���� ȸ���Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� �޺� 4��";
	tBrainInfo.szBrainEx[0] = L"���� ������ 4ȸ���� �����ؼ� �� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"2�� ���� ����";
	tBrainInfo.szBrainEx[0] = L"[C] ��� ������� ���� ���� ��, �� �ܰ� �� ���� ������";
	tBrainInfo.szBrainEx[1] = L"[C] �� ���� ������ ���ݡ� �� ������ ��������. ";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ���� ��ȭ";
	tBrainInfo.szBrainEx[0] = L"������ ���ݡ� �� ��2�� ���� ���ݡ���";
	tBrainInfo.szBrainEx[1] = L"���� ������ �о����� ������ ��������.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"���� �ļ�Ÿ";
	tBrainInfo.szBrainEx[0] = L"�ٿ�� ������ [������ ���콺]�� ���� ������ �ϸ�";
	tBrainInfo.szBrainEx[1] = L"����ġ���� ���� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 6;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� �޺� 5ź";
	tBrainInfo.szBrainEx[0] = L"���� ������ 5ȸ���� �����ؼ� �� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = GREEN;
	tBrainInfo.szBrainName = L"������ �������";
	tBrainInfo.szBrainEx[0] = L"��ó�� �������ִ� ��� �������� �ڵ����� �������.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"����Ʈ ȸ�� ����";
	tBrainInfo.szBrainEx[0] = L"����Ʈ ȸ�� ���� �� [���� ���콺]�� ���� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ����";
	tBrainInfo.szBrainEx[0] = L"���� [�����̽���]�� �ѹ� �� ������ �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"�߰� ���� Ȯ�� 1";
	tBrainInfo.szBrainEx[0] = L"���� �߰� ���ݰ� ���� �߰� ������";
	tBrainInfo.szBrainEx[1] = L"3ȸ���� �����ؼ� �� �� �ִ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"ȸ�� ���� ���� Ȯ��";
	tBrainInfo.szBrainEx[0] = L"[C]�� �̵��ϸ鼭 ���� ������ �۾���";
	tBrainInfo.szBrainEx[1] = L"��ȸ�� ���� ���ݡ���";
	tBrainInfo.szBrainEx[2] = L"�� �޺� �߿� 2������ �� �� �ִ�.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"���� �߰� ����";
	tBrainInfo.szBrainEx[0] = L"���� ���� �޺� �߿� [������ ���콺]�� ���� �߰� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"�߰� ���� Ȯ�� 2";
	tBrainInfo.szBrainEx[0] = L"���� �߰� ���ݰ� ���� �߰� ������";
	tBrainInfo.szBrainEx[1] = L"4ȸ���� �����ؼ� �� �� �ִ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ����";
	tBrainInfo.szBrainEx[0] = L"���� [��]�� ���� ������ �� �� �ִ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = EXPANSION;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ��� ����";
	tBrainInfo.szBrainEx[0] = L"������ ���ܡ� �� [���� ���콺]�� �����ϸ鼭 ���� ������ �Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// ����
	tBrainInfo.iBP = 1;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�÷����� ��� ���� �߰� 1";
	tBrainInfo.szBrainEx[0] = L"��� ȭ�鿡�� ������ �� �ִ� �÷������� �ϳ� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = GREEN;
	tBrainInfo.szBrainName = L"������ ��� ���� ����";
	tBrainInfo.szBrainEx[0] = L"������ ��� ���� ��� ���� �ð��� �����Ų��.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"�극�� ũ���� SAS ȸ��";
	tBrainInfo.szBrainEx[0] = L"SAS ȸ�� �߿� �극�� ũ���÷� ���� �����߸���";
	tBrainInfo.szBrainEx[1] = L"��� ���� SAS�� SAS �������� ���� ȸ���Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"�극�� ũ���� ȹ�� �ݾ� ���ʽ�";
	tBrainInfo.szBrainEx[0] = L"�극�� ũ���÷� ���� �����߸���";
	tBrainInfo.szBrainEx[1] = L"���� �����߷��� �� �Լ��� �� �ִ� ���� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"�극�� ũ���� ȹ�� EXP ���ʽ�";
	tBrainInfo.szBrainEx[0] = L"�극�� ũ���÷� ���� �����߸��� ȹ�� EXP�� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�÷����� ��� ���� �߰� 2";
	tBrainInfo.szBrainEx[0] = L"��� ȭ�鿡�� ������ �� �ִ� �÷������� �ϳ� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 5;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"SAS ���� �ߵ�";
	tBrainInfo.szBrainEx[0] = L"���� �̸��� ��ų�� 2�� ������,";
	tBrainInfo.szBrainEx[1] = L"�ϳ��� �����ϸ�, SAS�� ���ÿ� 2������ �ߵ��� �� ������,";
	tBrainInfo.szBrainEx[2] = L"2���� �����ϸ�, SAS�� ���ÿ� 4������ �ߵ��� �� �ִ�.";
	tBrainInfo.szBrainEx[3] = L"��SAS ��ȭ�� SAS ������ ���ÿ� �ߵ��� �� ����.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"���� ų";
	tBrainInfo.szBrainEx[0] = L"ũ���� �������� �� �￩ Ȱ���� ������";
	tBrainInfo.szBrainEx[1] = L"������ ������ ���߽�Ű�� EXP ���ʽ��� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"��, �극�� ũ���÷ιۿ� �����߸� �� ���� ������ ���,";
	tBrainInfo.szBrainEx[3] = L"ü���� �� ���� ��Ȳ������ ȿ���� ����.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = RAD;
	tBrainInfo.szBrainName = L"�극�� ũ���� ���� ����";
	tBrainInfo.szBrainEx[0] = L"���� ũ���� �������� �� ���� ��";
	tBrainInfo.szBrainEx[1] = L"���� �� ���� �ð� Ȱ���� �����ϰ� �Ǿ�";
	tBrainInfo.szBrainEx[2] = L"�극�� ũ���� ������ �ð��� �����Ѵ�.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"SAS ���� �ߵ�";
	tBrainInfo.szBrainEx[0] = L"���� �̸��� ��ų�� 2�� ������,";
	tBrainInfo.szBrainEx[1] = L"�ϳ��� �����ϸ�, SAS�� ���ÿ� 2������ �ߵ��� �� ������,";
	tBrainInfo.szBrainEx[2] = L"2���� �����ϸ�, SAS�� ���ÿ� 4������ �ߵ��� �� �ִ�.";
	tBrainInfo.szBrainEx[3] = L"��SAS ��ȭ�� SAS ������ ���ÿ� �ߵ��� �� ����.";
	m_vecBrain.push_back(tBrainInfo);
	// ��� �ٸ� ��

	// �극�� �ʵ�
	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 0;
	tBrainInfo.eType = BRAINFIELD;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: �ð� ���� 1";
	tBrainInfo.szBrainEx[0] = L"�극�� �ʵ��� ���� �ð��� 5�� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = BRAINFIELD;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: ���� ���� ���� ���";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� ��";
	tBrainInfo.szBrainEx[1] = L"[G] ��� ������� ���̴� ������Ʈ�� �����Ͽ�";
	tBrainInfo.szBrainEx[2] = L"������Ʈ�� ���Ͻ��Ѽ� �ִ� ������� �����Ѵ�.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// ���� _2
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = GREEN;
	tBrainInfo.szBrainName = L"������ ��� ȿ��";
	tBrainInfo.szBrainEx[0] = L"������ ��� ���� ������ ������";
	tBrainInfo.szBrainEx[1] = L"��� ȿ���� ���� �� �ִ�.";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 6;
	tBrainInfo.iLevel = 4;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"ü�� �ڵ� ȸ��";
	tBrainInfo.szBrainEx[0] = L"������ �� ü���� ������ ȸ���ȴ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	// �ٽ� �극�� �ʵ�
	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: ����� �氨";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� �� �޴� ��������";
	tBrainInfo.szBrainEx[1] = L"�氨�ȴ�. ���� �̸��� ��ų�� 2�� ������";
	tBrainInfo.szBrainEx[2] = L"1�� �����ϸ� ������� �ݰ�,";
	tBrainInfo.szBrainEx[3] = L"2�� �����ϸ� ����ڰ� ��ȿȭ�ȴ�.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: ���� ų";
	tBrainInfo.szBrainEx[0] = L"�극�� �ʵ� ���� ��, ũ���� �������� �� ����";
	tBrainInfo.szBrainEx[1] = L"������ ������ ���߽�Ű�� EXP ���ʽ��� �����Ѵ�.";
	tBrainInfo.szBrainEx[2] = L"��' �극�� ũ���÷ιۿ� �����߸� �� ���� ������ ���";
	tBrainInfo.szBrainEx[3] = L"ü���� �� ���� ��Ȳ������ ȿ���� ����.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: Ÿ�� ���� ���";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� ��";
	tBrainInfo.szBrainEx[1] = L"[���� ���콺]�� �ֵθ��� ������Ʈ�� �����Ͽ�";
	tBrainInfo.szBrainEx[2] = L"������� �� �� �ִ� ������ �о�����.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: ����̺� �� ���� �Һ� ����";
	tBrainInfo.szBrainEx[0] = L"�극�� �ʵ��� �غ� �������� ǥ�õ� ���¿���";
	tBrainInfo.szBrainEx[1] = L"����̺긦 �ߵ����� ���, ����̺� �� ������";
	tBrainInfo.szBrainEx[2] = L"�Һ����� �ʰ� �극�� �ʵ带 ������ �� �ִ�.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 5;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: ����� �氨";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� �� �޴� ��������";
	tBrainInfo.szBrainEx[1] = L"�氨�ȴ�. ���� �̸��� ��ų�� 2�� ������";
	tBrainInfo.szBrainEx[2] = L"1�� �����ϸ� ������� �ݰ�,";
	tBrainInfo.szBrainEx[3] = L"2�� �����ϸ� ����ڰ� ��ȿȭ�ȴ�.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 2;
	tBrainInfo.iLevel = 1;
	tBrainInfo.eType = BRAINFIELD;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: �߻� ���� ��� 1";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� ��,";
	tBrainInfo.szBrainEx[1] = L"[���� ���콺 2] [������ ���콺] �� ";
	tBrainInfo.szBrainEx[2] = L"[���� ���콺 3] [������ ���콺]�� [������ ���콺]��";
	tBrainInfo.szBrainEx[3] = L"������ ������Ʈ�� �����Ѵ�.";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 3;
	tBrainInfo.iLevel = 2;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = PURPLE;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: �߻� ���� ��� 2";
	tBrainInfo.szBrainEx[0] = L"�ڽ��� �극�� �ʵ带 �����ϰ� ���� ��";
	tBrainInfo.szBrainEx[1] = L"[���� ���콺 3] [������ ���콺]��";
	tBrainInfo.szBrainEx[2] = L"[������ ���콺]�� ������ ������Ʈ�� ���� �����Ѵ�.";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);

	tBrainInfo.iBP = 4;
	tBrainInfo.iLevel = 3;
	tBrainInfo.eType = ASSISTANCE;
	tBrainInfo.eColor = YELLOW;
	tBrainInfo.szBrainName = L"�극�� �ʵ�: �ð� ���� 2";
	tBrainInfo.szBrainEx[0] = L"�극�� �ʵ� ���� �ð��� 5�� �����Ѵ�.";
	tBrainInfo.szBrainEx[1] = L"";
	tBrainInfo.szBrainEx[2] = L"";
	tBrainInfo.szBrainEx[3] = L"";
	m_vecBrain.push_back(tBrainInfo);
}

void CCanvas_BrainMap::OnIcon_Tick()
{
	for (size_t i = 0; i < m_vecIconUI.size(); ++i)
	{
		// ������ ���� ���콺�� �ö��� ��
		if (true == m_vecIconUI[i]->Get_OnMouse())
		{
			Find_ChildUI(L"Icon_Pick")->Set_Position(m_vecIconUI[i]->Get_Position());
			m_BrainInfo = m_vecIconUI[i]->Get_BrainInfo();
		}

		// �������� Ŭ�� ���� ��
		if (true == m_vecIconUI[i]->Get_OnButton())
			dynamic_cast<CMain_BrainMapIconPickUI*>(Find_ChildUI(L"Icon_Pick"))->Set_Pick();
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

}
