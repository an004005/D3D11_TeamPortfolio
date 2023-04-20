#include "stdafx.h"
#include "..\public\DistanceUI.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "UI_Manager.h"

#include "Player.h"
#include "Canvas_Main.h"
#include "Canvas_Shop.h"
#include "Canvas_WorldMap.h"

CDistanceUI::CDistanceUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CDistanceUI::CDistanceUI(const CDistanceUI& rhs)
	: CUI(rhs)
{
}

HRESULT CDistanceUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDistanceUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	CLayer* pLayer = CGameInstance::GetInstance()->GetLayer(LEVEL_NOW, L"Layer_Player");
	if (pLayer != nullptr)
	{
		list<CGameObject*> plsGameObject = pLayer->GetGameObjects();

		auto iter = find_if(plsGameObject.begin(), plsGameObject.end(), [](CGameObject* pair) {
			return pair->GetPrototypeTag() == L"Player";
			});

		if (iter != plsGameObject.end())
		{
			m_pPlayer = dynamic_cast<CPlayer*>(*iter);
			Assert(m_pPlayer != nullptr);
		}
	}

	return S_OK;
}

void CDistanceUI::Tick(_double TimeDelta)
{
	if (dynamic_cast<CCanvas_Main*>(CUI_Manager::GetInstance()->Find_WindowCanvas(L"CCanvas_Main"))->Get_BeSeen() ||
		dynamic_cast<CCanvas_Shop*>(CUI_Manager::GetInstance()->Find_WindowCanvas(L"CCanvas_Shop"))->Get_BeSeen() ||
		dynamic_cast<CCanvas_WorldMap*>(CUI_Manager::GetInstance()->Find_WindowCanvas(L"CCanvas_WorldMap"))->Get_BeSeen())
	{
		m_bVisible = false;
		return;
	}
	else
	{
		m_bVisible = true;
	}

	__super::Tick(TimeDelta);

	_vector vPlayerPos = m_pPlayer->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
	_vector	vDistance = vPlayerPos - XMVectorSet(m_vArrivalPoint.x, m_vArrivalPoint.y, m_vArrivalPoint.z, m_vArrivalPoint.w);
	m_fDistance = XMVectorGetX(XMVector3Length(vDistance));

	if (7.0f > m_fDistance)
		CGameObject::SetDelete();
}

HRESULT CDistanceUI::Render()
{
	if (FAILED(CUI::Render()))
		return E_FAIL;

	_tchar szText[MAX_PATH] = TEXT("");
	_float2 vFontSize = { 0.5f, 0.5f };
	_float4 vColor = { 1.0f, 0.458f, 0.38f, 1.0f };
	_float2 vPosition = GetScreenSpaceLeftTop();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	wsprintf(szText, TEXT("%dm"), static_cast<_int>(m_fDistance));
	pGameInstance->Render_Font(L"Pretendard32", szText, vPosition + _float2(45.0f, 7.0f), 0.f, vFontSize, vColor);

	return E_NOTIMPL;
}

void CDistanceUI::LoadFromJson(const Json& json)
{
	__super::LoadFromJson(json);

	m_vArrivalPoint = json["ArrivalPoint"];
}

CDistanceUI * CDistanceUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CDistanceUI*		pInstance = new CDistanceUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDistanceUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CDistanceUI::Clone(void * pArg)
{
	CDistanceUI*		pInstance = new CDistanceUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CDistanceUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDistanceUI::Free()
{
	__super::Free();

}
