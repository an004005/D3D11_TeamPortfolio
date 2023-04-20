#include "stdafx.h"
#include "..\public\SASSkillFullCircleUI.h"
#include "GameInstance.h"
#include "PlayerInfoManager.h"

// 알파값이 줄어들면서, 원이 커진다.

CSASSkillFullCircleUI::CSASSkillFullCircleUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUI(pDevice, pContext)
{
}

CSASSkillFullCircleUI::CSASSkillFullCircleUI(const CSASSkillFullCircleUI& rhs)
	: CUI(rhs)
{
}

HRESULT CSASSkillFullCircleUI::Initialize_Prototype()
{
	if (FAILED(CUI::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSASSkillFullCircleUI::Initialize(void * pArg)
{
	if (FAILED(CUI::Initialize(pArg)))
		return E_FAIL;

	// 1 ~ 7
	static _uint iCount = 0;
	++iCount;
	m_iObjectNum = iCount;

	if (7 == iCount)
		iCount = 0;

	m_vOriginSize = { m_fSizeX * m_vScale.x, m_fSizeY * m_vScale.y };
	m_vCurrentSize = { 170.0f, 170.0f };	// 최대 크기를 이야기 한다.
	m_fSpeed = 50.0f; 

	m_bVisible = true;
	m_bOneGrow = true;

	return S_OK;
}

void CSASSkillFullCircleUI::Tick(_double TimeDelta)
{
	CUI::Tick(TimeDelta);
	
	Object_Tick();
	Grow_Tick(TimeDelta);
}

void CSASSkillFullCircleUI::Grow_Tick(const _double& TimeDelta)
{
	if (false == m_bGrow) return;

	if (m_vOriginSize.x < m_vCurrentSize.x)
	{
		m_vCurrentSize -= {_float(TimeDelta)* m_fSpeed, _float(TimeDelta)* m_fSpeed};
		m_pTransformCom->Set_Scaled(_float3(m_vCurrentSize.x, m_vCurrentSize.y, 1.f));
	}
	else
	{
		m_bGrow = false;
		m_vCurrentSize = { 140.0f, 140.0f };
		m_pTransformCom->Set_Scaled(_float3(m_vOriginSize.x, m_vOriginSize.y, 1.f));
	}
}

void CSASSkillFullCircleUI::Object_Tick()
{
	if (false == m_bChangeX)
	{
		if (1 == m_iObjectNum)
		{
			if (false == m_bTelepotiCircle) return;

			_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(ESASType::SAS_TELEPORT)].Energy;
			_float fMinEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(ESASType::SAS_TELEPORT)].MinEnergy;

			if (fCurrentEnergy > fMinEnergy)
			{
				m_bVisible = true;
				if (false == m_bOneGrow)
				{
					m_bOneGrow = true;
					m_bGrow = true;
				}
			}
			else
			{
				m_bVisible = false;
				m_bOneGrow = false;
			}
		}
		else if (2 == m_iObjectNum)
		{
			_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(ESASType::SAS_PENETRATE)].Energy;
			_float fMinEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(ESASType::SAS_PENETRATE)].MinEnergy;

			if (fCurrentEnergy > fMinEnergy)
			{
				m_bVisible = true;
				if (false == m_bOneGrow)
				{
					m_bOneGrow = true;
					m_bGrow = true;
				}
			}
			else
			{
				m_bVisible = false;
				m_bOneGrow = false;
			}
		}
		else if (3 == m_iObjectNum)
		{
			_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(ESASType::SAS_HARDBODY)].Energy;
			_float fMinEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(ESASType::SAS_HARDBODY)].MinEnergy;

			if (fCurrentEnergy > fMinEnergy)
			{
				m_bVisible = true;
				if (false == m_bOneGrow)
				{
					m_bOneGrow = true;
					m_bGrow = true;
				}
			}
			else
			{
				m_bVisible = false;
				m_bOneGrow = false;
			}
		}
		else if (4 == m_iObjectNum)
		{
			_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(ESASType::SAS_FIRE)].Energy;
			_float fMinEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(ESASType::SAS_FIRE)].MinEnergy;

			m_bVisible = true;
			if (fCurrentEnergy > fMinEnergy)
			{
				if (false == m_bOneGrow)
				{
					m_bOneGrow = true;
					m_bGrow = true;
				}
			}
			else
			{
				m_bVisible = false;
				m_bOneGrow = false;
			}
		}
	}
	else
	{
		if (5 == m_iObjectNum)
		{
			_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(ESASType::SAS_SUPERSPEED)].Energy;
			_float fMinEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(ESASType::SAS_SUPERSPEED)].MinEnergy;

			if (fCurrentEnergy > fMinEnergy)
			{
				m_bVisible = true;
				if (false == m_bOneGrow)
				{
					m_bOneGrow = true;
					m_bGrow = true;
				}
			}
			else
			{
				m_bVisible = false;
				m_bOneGrow = false;
			}
		}
		else if (6 == m_iObjectNum)
		{
			_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(ESASType::SAS_ELETRIC)].Energy;
			_float fMinEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(ESASType::SAS_ELETRIC)].MinEnergy;

			if (fCurrentEnergy > fMinEnergy)
			{
				m_bVisible = true;
				if (false == m_bOneGrow)
				{
					m_bOneGrow = true;
					m_bGrow = true;
				}
			}
			else
			{
				m_bVisible = false;
				m_bOneGrow = false;
			}
		}
		else if (7 == m_iObjectNum)
		{
			_float fCurrentEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(ESASType::SAS_COPY)].Energy;
			_float fMinEnergy = CPlayerInfoManager::GetInstance()->Get_PlayerStat().Sasese[_int(ESASType::SAS_COPY)].MinEnergy;

			if (fCurrentEnergy > fMinEnergy)
			{
				m_bVisible = true;
				if (false == m_bOneGrow)
				{
					m_bOneGrow = true;
					m_bGrow = true;
				}
			}
			else
			{
				m_bVisible = false;
				m_bOneGrow = false;
			}
		}
	}
}

CSASSkillFullCircleUI * CSASSkillFullCircleUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
 	CSASSkillFullCircleUI*		pInstance = new CSASSkillFullCircleUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSASSkillFullCircleUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CUI * CSASSkillFullCircleUI::Clone(void * pArg)
{
	CSASSkillFullCircleUI*		pInstance = new CSASSkillFullCircleUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSASSkillFullCircleUI");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSASSkillFullCircleUI::Free()
{
	CUI::Free();

}
