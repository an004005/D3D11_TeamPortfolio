#include "stdafx.h"
#include "CMaterial_AnimBase.h"
#include "Shader.h"
#include "Texture.h"
#include "JsonLib.h"
#include "GameInstance.h"
#include "GameUtils.h"

CMaterial_AnimBase::CMaterial_AnimBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMaterial(pDevice, pContext)
{
}

CMaterial_AnimBase::CMaterial_AnimBase(const CMaterial_AnimBase& rhs)
	: CMaterial(rhs)
	, m_pDiffuse(rhs.m_pDiffuse)
	, m_pMRAE(rhs.m_pMRAE)
	, m_pNormal(rhs.m_pNormal)
{
	Safe_AddRef(m_pDiffuse);
	Safe_AddRef(m_pMRAE);
	Safe_AddRef(m_pNormal);
}

HRESULT CMaterial_AnimBase::Initialize_Prototype(const Json& json, const char* pTexDir)
{
	// FAILED_CHECK(__super::Initialize_Prototype());
	const string texDir = pTexDir;

	if (json.contains("Diffuse"))
	{
		const string texPath = texDir + json["Diffuse"].get<string>();
		m_pDiffuse = dynamic_cast<CTexture*>(CGameInstance::GetInstance()->Clone_Component(CGameUtils::s2ws(texPath).c_str()));
		m_tParam.ParamTexture[0] = { m_pDiffuse, 0 };
	}

	if (json.contains("MRAE"))
	{
		const string texPath = texDir + json["MRAE"].get<string>();
		m_pMRAE = dynamic_cast<CTexture*>(CGameInstance::GetInstance()->Clone_Component(CGameUtils::s2ws(texPath).c_str()));
		m_tParam.ParamTexture[1] = { m_pMRAE, 0 };
	}

	if (json.contains("Normal"))
	{
		const string texPath = texDir + json["Normal"].get<string>();
		m_pNormal = dynamic_cast<CTexture*>(CGameInstance::GetInstance()->Clone_Component(CGameUtils::s2ws(texPath).c_str()));
		m_tParam.ParamTexture[2] = { m_pNormal, 0 };
	}

	return S_OK;
}

CMaterial_AnimBase* CMaterial_AnimBase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const Json& json,
	const char* pTexDir)
{
	CMaterial_AnimBase*		pInstance = new CMaterial_AnimBase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(json, pTexDir)))
	{
		MSG_BOX("Failed to Created : CShader");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CMaterial_AnimBase::Clone(void* pArg)
{
	CMaterial_AnimBase*		pInstance = new CMaterial_AnimBase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CValorantMaterial_Base");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMaterial_AnimBase::Free()
{
	__super::Free();
	Safe_Release(m_pDiffuse);
	Safe_Release(m_pMRAE);
	Safe_Release(m_pNormal);
}

void CMaterial_AnimBase::SetAnimType(CModel::TYPE eType)
{
	if (eType == CModel::TYPE_NONANIM)
	{
		m_pShader = dynamic_cast<CShader*>(CGameInstance::GetInstance()->Clone_Component(L"Prototype_Component_Shader_VtxModel"));
		Assert(m_pShader);
		m_iPassIdx = 3;
	}
	else
	{
		m_pShader = dynamic_cast<CShader*>(CGameInstance::GetInstance()->Clone_Component(L"Prototype_Component_Shader_VtxAnimModel"));
		Assert(m_pShader);
		m_iPassIdx = 1;
	}
}

void CMaterial_AnimBase::SetShaderResources()
{
	FAILED_CHECK(m_pShader->Set_Params(m_tParam));
}
