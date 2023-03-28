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

	return S_OK;
}

HRESULT CCanvas_Acquisition::Initialize(void* pArg)
{
	if (FAILED(CCanvas::Initialize(pArg)))
		return E_FAIL;

	CUI_Manager::GetInstance()->Add_Canvas(L"Canvas_Acquisition", this);

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
	Json json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/UI/UI_PositionData/AcquisitionUI.json");
#pragma region EnemyName
	_tchar* szEnemyName = L"";
	switch (eEnemyName)
	{
	case EEnemyName::EM0110:
		szEnemyName = L"오거 사바트";
		break;
	case EEnemyName::EM0200:
		szEnemyName = L"바스 포즈";
		break;
	case EEnemyName::EM0210:
		szEnemyName = L"소스 포즈";
		break;
	case EEnemyName::EM0220:
		szEnemyName = L"베이스 포즈";
		break;
	case EEnemyName::EM0320:
		szEnemyName = L"경건 페리";
		break;
	case EEnemyName::EM0400:
		szEnemyName = L"버디 러미";
		break;
	case EEnemyName::EM0650:
		szEnemyName = L"바일 풀";
		break;
	case EEnemyName::EM0700:
		szEnemyName = L"방도 팡뒤";
		break;
	case EEnemyName::EM0750:
		szEnemyName = L"스커미 팡뒤";
		break;
	case EEnemyName::EM0800:
		szEnemyName = L"브론 욘";
		break;
	case EEnemyName::EM1100:
		szEnemyName = L"와이너리 치너리";
		break;
	case EEnemyName::EM1200:
		szEnemyName = L"나오미 랜들";
		break;
	case EEnemyName::EM8210:
		szEnemyName = L"중대장";
		break;
	default:
		szEnemyName = L"잘못된 이름";
		break;
	}
#pragma endregion EnemyName

	_tchar szText[MAX_PATH] = TEXT("");
	wsprintf(szText, TEXT("%s를 쓰러뜨렸다."), szEnemyName);

	json["Text"] = ws2s(szText);
	CAcquisitionUI* pUI = dynamic_cast<CAcquisitionUI*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"AcquisitionUI", &json));
	assert(pUI != nullptr && "Failed to Clone : CAcquisitionUI");

	m_AcquisitionUIList.push_back(pUI);

	_uint iExp = CMathUtils::RandomUInt(iLevel) + iLevel * 4;

	// 추가적으로 플레이어에 정보도 Set 해주기
	CPlayerInfoManager::GetInstance()->Get_PlayerStat().iExp += iExp;

	wsprintf(szText, TEXT("%uEXP 입수"), iExp);

	json["Text"] = ws2s(szText);
	pUI = dynamic_cast<CAcquisitionUI*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"AcquisitionUI", &json));
	assert(pUI != nullptr && "Failed to Clone : CAcquisitionUI");

	m_AcquisitionUIList.emplace_back(pUI);

	_uint iCoin = min(iLevel * 1, CMathUtils::RandomUInt(iLevel * 3));

	// 추가적으로 플레이어에 정보도 Set 해주기
	CPlayerInfoManager::GetInstance()->Get_PlayerStat().iCoin += iCoin;

	wsprintf(szText, TEXT("K %u입수"), iCoin);

	json["Text"] = ws2s(szText);
	pUI = dynamic_cast<CAcquisitionUI*>(CGameInstance::GetInstance()->Clone_GameObject_Get(PLAYERTEST_LAYER_FRONTUI, L"AcquisitionUI", &json));
	assert(pUI != nullptr && "Failed to Clone : CAcquisitionUI");

	m_AcquisitionUIList.emplace_back(pUI);
}

void CCanvas_Acquisition::Set_Visible()
{
	for (auto iter = m_AcquisitionUIList.begin(); iter != m_AcquisitionUIList.end(); ++iter)
	{
		(*iter)->SetVisible(false);
	}
}

void CCanvas_Acquisition::EnemyUIAdd_Tick(const _double & TimeDelta)
{
	if (0 == m_AcquisitionUIList.size()) return;

	if (0.0 == m_dEnemyClone_TimeAcc)
	{
		for (auto iter = m_AcquisitionUIList.begin(); iter != m_AcquisitionUIList.end(); ++iter)
		{
			if ((*iter)->Get_Renning() == false)
			{
				AllPositionChange();
				(*iter)->Set_Renning();
				break;
			}
		}
	}

	m_dEnemyClone_TimeAcc += TimeDelta;
	if (1.0 < m_dEnemyClone_TimeAcc)
	{
		m_dEnemyClone_TimeAcc = 0.0;
	}
}

void CCanvas_Acquisition::AllPositionChange()
{
	for (auto iter : m_AcquisitionUIList)
	{
		if (false == iter->Get_Renning())
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