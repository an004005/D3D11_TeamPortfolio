#include "stdafx.h"
#include "..\public\Canvas_Acquisition.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "MathUtils.h"
#include "UI_Manager.h"
#include "PlayerInfoManager.h"

#include "AcquisitionUI.h"

CCanvas_Acquisition::CCanvas_Acquisition(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCanvas(pDevice, pContext)
{
}

CCanvas_Acquisition::CCanvas_Acquisition(const CCanvas_Acquisition& rhs)
	: CCanvas(rhs)
{
}

HRESULT CCanvas_Acquisition::Initialize_Prototype()
{
	if (FAILED(CCanvas::Initialize_Prototype()))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_Canvas(L"Canvas_Acquisition", this);

	return S_OK;
}

HRESULT CCanvas_Acquisition::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	for (map<wstring, CUI*>::iterator iter = m_mapChildUIs.begin(); iter != m_mapChildUIs.end(); ++iter)
		iter->second->SetVisible(false);

	return S_OK;
}

void CCanvas_Acquisition::Tick(_double TimeDelta)
{
	CCanvas::Tick(TimeDelta);

	EnemyUIAdd_Tick(TimeDelta);
	DeleteUI();
}

void CCanvas_Acquisition::Late_Tick(_double TimeDelta)
{
	CCanvas::Late_Tick(TimeDelta);

}

HRESULT CCanvas_Acquisition::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;


	return S_OK;
}

void CCanvas_Acquisition::Imgui_RenderProperty()
{
	CCanvas::Imgui_RenderProperty();
}

void CCanvas_Acquisition::SaveToJson(Json& json)
{
	CCanvas::SaveToJson(json);
}

void CCanvas_Acquisition::LoadFromJson(const Json & json)
{
	CCanvas::LoadFromJson(json);
}

void CCanvas_Acquisition::Set_EnemyUI(const EEnemyName eEnemyName, const _uint iLevel)
{
	m_EEnemyName = eEnemyName;
	m_iLevel = iLevel;
	m_bEnemyClone[0] = true;
	m_bEnemyClone[1] = false;
	m_bEnemyClone[2] = false;
	m_bEnemyClone[3] = false;
	m_dEnemyClone_TimeAcc = 0.0;
}

void CCanvas_Acquisition::EnemyUIAdd_Tick(const _double & TimeDelta)
{
	if (false == m_bEnemyClone[0]) return;

	m_dEnemyClone_TimeAcc += TimeDelta;

	if (0.0 < m_dEnemyClone_TimeAcc)
	{
		// Name
		if (m_bEnemyClone[1] == false)
		{
			m_bEnemyClone[1] = true;

			Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/AcquisitionUI.json");
#pragma region EnemyName
			_tchar* szEnemyName = L"";
			switch (m_EEnemyName)
			{
			case EEnemyName::EM0110:
				szEnemyName = L"���� ���Ʈ";
				break;
			case EEnemyName::EM0200:
				szEnemyName = L"�ٽ� ����";
				break;
			case EEnemyName::EM0210:
				szEnemyName = L"�ҽ� ����";
				break;
			case EEnemyName::EM0220:
				szEnemyName = L"���̽� ����";
				break;
			case EEnemyName::EM0320:
				szEnemyName = L"��� �丮";
				break;
			case EEnemyName::EM0400:
				szEnemyName = L"���� ����";
				break;
			case EEnemyName::EM0650:
				szEnemyName = L"���� Ǯ";
				break;
			case EEnemyName::EM0700:
				szEnemyName = L"�浵 �ε�";
				break;
			case EEnemyName::EM0750:
				szEnemyName = L"��Ŀ�� �ε�";
				break;
			case EEnemyName::EM0800:
				szEnemyName = L"��� ��";
				break;
			case EEnemyName::EM1100:
				szEnemyName = L"���̳ʸ� ġ�ʸ�";
				break;
			case EEnemyName::EM1200:
				szEnemyName = L"������ ����";
				break;
			case EEnemyName::EM8210:
				szEnemyName = L"�ߴ���";
				break;
			default:
				szEnemyName = L"�߸��� �̸�";
				break;
			}
#pragma endregion EnemyName
			
			_tchar szText[MAX_PATH] = TEXT("");
			wsprintf(szText, TEXT("%s�� �����߷ȴ�."), szEnemyName);

			json["Text"] = ws2s(szText);
			CAcquisitionUI* pUI = dynamic_cast<CAcquisitionUI*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"AcquisitionUI", &json));
			assert(pUI != nullptr && "Failed to Clone : CAcquisitionUI");

			m_AcquisitionUIList.push_back(pUI);
			AllPositionChange();
		}
	}

	if (1.0 < m_dEnemyClone_TimeAcc)
	{
		// Exp
		if (m_bEnemyClone[2] == false)
		{
			m_bEnemyClone[2] = true;

			_uint iExp = CMathUtils::RandomUInt(m_iLevel) + m_iLevel * 4;

			_tchar szText[MAX_PATH] = TEXT("");
			wsprintf(szText, TEXT("%uEXP �Լ�"), iExp);

			Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/AcquisitionUI.json");
			json["Text"] = ws2s(szText);
			CAcquisitionUI* pUI = dynamic_cast<CAcquisitionUI*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"AcquisitionUI", &json));
			assert(pUI != nullptr && "Failed to Clone : CAcquisitionUI");

			m_AcquisitionUIList.emplace_back(pUI);
			AllPositionChange();
		}
	}
	
	if (2.0 < m_dEnemyClone_TimeAcc)
	{
		// Coin
		if (m_bEnemyClone[3] == false)
		{
			m_bEnemyClone[3] = true;
			m_bEnemyClone[0] = false;

			_uint iCoin = min(m_iLevel * 1, CMathUtils::RandomUInt(m_iLevel * 3));

			_tchar szText[MAX_PATH] = TEXT("");
			wsprintf(szText, TEXT("K %u�Լ�"), iCoin);

			Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/AcquisitionUI.json");
			json["Text"] = ws2s(szText);
			CAcquisitionUI* pUI = dynamic_cast<CAcquisitionUI*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"AcquisitionUI", &json));
			assert(pUI != nullptr && "Failed to Clone : CAcquisitionUI");

			m_AcquisitionUIList.emplace_back(pUI);
			AllPositionChange();
		}
	}
}

void CCanvas_Acquisition::AllPositionChange()
{
	CAcquisitionUI* pUIBack = m_AcquisitionUIList.back();

	for (auto iter : m_AcquisitionUIList)
	{
		if (iter == pUIBack)
			continue;

		_float2 fTempPosition = iter->Get_Position();
		fTempPosition.y += 22.0f;
		iter->Set_Position(fTempPosition);

	}
}

void CCanvas_Acquisition::DeleteUI()
{
	auto ui = find_if(m_AcquisitionUIList.begin(), m_AcquisitionUIList.end(), [](CAcquisitionUI* pUI)
	{
		return pUI->Get_Dead();
	});

	if (ui != m_AcquisitionUIList.end())
	{
		(*ui)->SetDelete();
		m_AcquisitionUIList.erase(ui);
	}
}

CCanvas_Acquisition * CCanvas_Acquisition::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCanvas_Acquisition*      pInstance = new CCanvas_Acquisition(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCanvas_Acquisition");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CCanvas * CCanvas_Acquisition::Clone(void * pArg)
{
	CCanvas_Acquisition*      pInstance = new CCanvas_Acquisition(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCanvas_Acquisition");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCanvas_Acquisition::Free()
{
	CCanvas::Free();

	m_AcquisitionUIList.clear();
}