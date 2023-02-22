#include "stdafx.h"
#include "..\public\Player.h"
#include "GameInstance.h"
#include "Shader.h"
#include "AnimationInstance.h"
#include "Animation.h"
#include "FSMComponent.h"
#include "BaseAnimInstance.h"
#include "Model.h"
#include "JsonStorage.h"
#include "GameUtils.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	:CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	m_pTransformCom->SetTransformDesc({ 1.f, XMConvertToRadians(90.f) });

	return S_OK;
}

void CPlayer::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->KeyPressing(DIK_UP))
	{
		m_bWalk = true;
	}
	else
	{
		m_bWalk = false;
	}

	m_pASM->Tick(TimeDelta);
}

void CPlayer::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bVisible && (nullptr != m_pRenderer))
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModel->Render(m_pTransformCom);

	return S_OK;
}

HRESULT CPlayer::SetUp_Components(void * pArg)
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRenderer));

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		if (json.contains("Model"))
		{
			string ProtoModel = json["Model"];
			m_ModelName = CGameUtils::s2ws(ProtoModel);
			FAILED_CHECK(__super::Add_Component(LEVEL_NOW, m_ModelName.c_str(), m_ModelName.c_str(),
				(CComponent**)&m_pModel));
		}
	}

	m_pASM = CBaseAnimInstance::Create(m_pModel, this);
	if (nullptr == m_pASM)
		MSG_BOX("FAIL");

	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CPlayer");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Release(m_pFSM);
	Safe_Release(m_pASM);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pModel);
}
