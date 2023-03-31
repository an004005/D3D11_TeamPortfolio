#include "stdafx.h"
#include "..\public\Special_TankLorry.h"
#include "JsonStorage.h"
#include "GameUtils.h"
#include "GameInstance.h"
#include "Model.h"
#include "RigidBody.h"
#include "MathUtils.h"
#include "Animation.h"
#include "ImguiUtils.h"
#include "MapObject.h"
#include "Enemy.h"
#include "PlayerInfoManager.h"

#include "Special_TankLorry_Head.h"
#include "Special_TankLorry_Trailer.h"

#include "ParticleGroup.h"

CSpecial_TankLorry::CSpecial_TankLorry(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CSpecialObject(pDevice, pContext)
{
}

CSpecial_TankLorry::CSpecial_TankLorry(const CSpecial_TankLorry & rhs)
	: CSpecialObject(rhs)
{
}

HRESULT CSpecial_TankLorry::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSpecial_TankLorry::Initialize(void * pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(SetUp_Components(pArg));

	m_pTransformCom->SetTransformDesc({ 0.1f, XMConvertToRadians(90.f) });

	m_eSpecialObejctType = SPECIAL_TANKLORRY;

	Create_ChildObject();

	return S_OK;
}

void CSpecial_TankLorry::BeginTick()
{
	__super::BeginTick();
}

void CSpecial_TankLorry::Tick(_double TimeDelta)
{
	if (m_bDeadCheck)
	{
		static_cast<CSpecial_TankLorry_Head*>(m_pTankLorry_Head)->SetOutline(false);
		static_cast<CSpecial_TankLorry_Trailer*>(m_pTankLorry_Trailer)->SetOutline(false);

		m_fDeadTime -= (_float)TimeDelta;

		if (0.f >= m_fDeadTime)
		{
			m_pTankLorry_Head->SetDelete();
			m_pTankLorry_Trailer->SetDelete();

			this->SetDelete();
		}
	}
	else
	{
		if (CPlayerInfoManager::GetInstance()->Get_SpecialObject() == this)
		{
			static_cast<CSpecial_TankLorry_Head*>(m_pTankLorry_Head)->SetOutline(true);
			static_cast<CSpecial_TankLorry_Trailer*>(m_pTankLorry_Trailer)->SetOutline(true);
		}
		else
		{
			static_cast<CSpecial_TankLorry_Head*>(m_pTankLorry_Head)->SetOutline(false);
			static_cast<CSpecial_TankLorry_Trailer*>(m_pTankLorry_Trailer)->SetOutline(false);
		}
	}

	__super::Tick(TimeDelta);

	m_pTankLorry_Head->Tick(TimeDelta);
	m_pTankLorry_Trailer->Tick(TimeDelta);

	static_cast<CSpecial_TankLorry_Head*>(m_pTankLorry_Head)->Set_ChildMatrix(m_HeadMatrix);
	static_cast<CSpecial_TankLorry_Head*>(m_pTankLorry_Head)->Sync_position(m_pTransformCom);
	static_cast<CSpecial_TankLorry_Trailer*>(m_pTankLorry_Trailer)->Set_ChildMatrix(m_TrailerMatrix);
	static_cast<CSpecial_TankLorry_Trailer*>(m_pTankLorry_Trailer)->Sync_position(m_pTransformCom);
}

void CSpecial_TankLorry::Late_Tick(_double TimeDelta)
{
	m_pTankLorry_Head->Late_Tick(TimeDelta);
	m_pTankLorry_Trailer->Late_Tick(TimeDelta);
}

void CSpecial_TankLorry::AfterPhysX()
{
	m_pTankLorry_Head->AfterPhysX();
	m_pTankLorry_Trailer->AfterPhysX();
}

HRESULT CSpecial_TankLorry::Render()
{
	return S_OK;
}

void CSpecial_TankLorry::LoadFromJson(const Json & json)
{
	__super::LoadFromJson(json);

	if (json.contains("ModelTags"))
	{
		for (auto tag : json["ModelTags"])
			m_pModelTags.push_back(s2ws(tag));
	}

	if (json.contains("LocalMatrix"))
		m_LocalMatrix = json["LocalMatrix"];

	if (json.contains("HeadMatrix"))
		m_HeadMatrix = json["HeadMatrix"];

	if (json.contains("TrailerMatrix"))
		m_TrailerMatrix = json["TrailerMatrix"];
}

void CSpecial_TankLorry::SaveToJson(Json & json)
{
	__super::SaveToJson(json);

	for (auto tag : m_pModelTags)
		json["ModelTags"].push_back(ws2s(tag));

	json["LocalMatrix"] = m_LocalMatrix;

	json["HeadMatrix"] = m_HeadMatrix;

	json["TrailerMatrix"] = m_TrailerMatrix;
}

void CSpecial_TankLorry::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	if (ImGui::CollapsingHeader("TruckHead"))
	{
		static GUIZMO_INFO Head;
		CImguiUtils::Render_Guizmo(&m_HeadMatrix, Head, true, true);
	}

	if (ImGui::CollapsingHeader("Trailer"))
	{
		static GUIZMO_INFO Trailer;
		CImguiUtils::Render_Guizmo(&m_TrailerMatrix, Trailer, true, true);
	}

	if (ImGui::Button("Activate"))
	{
		TankLorry_Activate();
	}

	static _bool bShake = false;
	ImGui::Checkbox("Shake", &bShake);

	if (bShake)
	{
		TankLorry_Shake(20.f);
	}

	if (ImGui::Button("Explosion"))
	{
		TankLorry_Explosion();
	}
}

void CSpecial_TankLorry::TankLorry_Activate()
{
	static_cast<CSpecial_TankLorry_Head*>(m_pTankLorry_Head)->Set_Kinetic(false);
	static_cast<CSpecial_TankLorry_Trailer*>(m_pTankLorry_Trailer)->Set_Kinetic(false);

	//m_pChargeParticle = CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Special_G_Truck_Kinetic_Particles");
	//m_pChargeParticle->Start_AttachPosition(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), XMVectorSet(0.f, 1.f, 0.f, 0.f), false);
	//Special_G_Truck_Kinetic_Particles
}

void CSpecial_TankLorry::TankLorry_Shake(_float fRange)
{
	static_cast<CSpecial_TankLorry_Head*>(m_pTankLorry_Head)->Shake(fRange);
	static_cast<CSpecial_TankLorry_Trailer*>(m_pTankLorry_Trailer)->Shake(fRange);
}

void CSpecial_TankLorry::TankLorry_Bounce(_float fForce)
{
	static_cast<CSpecial_TankLorry_Head*>(m_pTankLorry_Head)->Bounce(fForce);
	static_cast<CSpecial_TankLorry_Trailer*>(m_pTankLorry_Trailer)->Bounce(fForce);
}

void CSpecial_TankLorry::TankLorry_Explosion()
{
	static_cast<CSpecial_TankLorry_Head*>(m_pTankLorry_Head)->Exploision();
	static_cast<CSpecial_TankLorry_Trailer*>(m_pTankLorry_Trailer)->Exploision();

	CGameInstance::GetInstance()->SetTimeRatioCurve("TankLorry_Slow");
//	CGameInstance::GetInstance()->AddLifePointLight(3.f, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), 10.f, { 1.f, 0.f, 0.f, 1.f });

	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pChargeParticle))
	{
		m_pChargeParticle->Delete_Particles();
		m_pChargeParticle = nullptr;
	}

	m_bDeadCheck = true;
	m_fDeadTime = 6.f;
}

void CSpecial_TankLorry::TankLorry_Cheage_TankIndex(_uint iIndex)
{
	static_cast<CSpecial_TankLorry_Trailer*>(m_pTankLorry_Trailer)->Change_Tank(iIndex);
}

void CSpecial_TankLorry::TankLorry_Create_OilParticle()
{
	static_cast<CSpecial_TankLorry_Trailer*>(m_pTankLorry_Trailer)->Create_Oil_Particle();
}

void CSpecial_TankLorry::TankLorry_Release_OilParticle()
{
	static_cast<CSpecial_TankLorry_Trailer*>(m_pTankLorry_Trailer)->Release_Oil_Particle();
}

void CSpecial_TankLorry::TankLorry_Explosion_Effect()
{
	CVFX_Manager::GetInstance()->GetEffect(EFFECT::EF_SAS, L"Special_G_Truck_Explode_Base")->
		Start_AttachPosition_Scale(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION) + XMVectorSet(0.f, 2.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f), _float4(2.f, 2.f, 2.f, 0.f), false);
}

void CSpecial_TankLorry::TankLorry_Explosion_Particle()
{
	CVFX_Manager::GetInstance()->GetParticle(PARTICLE::PS_SAS, L"Truck_Explode")->
		Start_AttachPosition(this, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), XMVectorSet(0.f, 1.f, 0.f, 0.f), false);
}

HRESULT CSpecial_TankLorry::SetUp_Components(void * pArg)
{
	return S_OK;
}

void CSpecial_TankLorry::Create_ChildObject()
{
	{	// 헤드 ************************************************************************** //
		wstring wstrLayer = L"Layer_Kinetic";
		string strJsonPath = "../Bin/Resources/Objects/KineticSpecial_TankLorry_Head.json";
		wstring wstrProtoTag = L"Prototype_GameObject_Special_TankLorry_Head";

		auto pProtoType = CGameInstance::GetInstance()->Find_Prototype(LEVEL_NOW, wstrProtoTag.c_str());
		if (pProtoType == nullptr)
			pProtoType = CGameInstance::GetInstance()->Find_Prototype(LEVEL_STATIC, wstrProtoTag.c_str());

		if (pProtoType == nullptr)
		{
			MSG_BOX("ProtoType is not exist");
		}
		else if (wstrLayer.empty())
		{
			MSG_BOX("Layer is empty");
		}
		else if (strJsonPath.empty() == false && CGameUtils::FileExist(s2ws(strJsonPath).c_str()) == false)
		{
			MSG_BOX("Json file is not exist");
		}
		else
		{
			if (strJsonPath.empty() == false)
			{
				Json json = CJsonStorage::GetInstance()->FindOrLoadJson(strJsonPath);
				m_pTankLorry_Head = CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, wstrProtoTag.c_str(), &json);
			}
			else
			{
				m_pTankLorry_Head = CGameInstance::GetInstance()->Clone_GameObject_Get(wstrLayer.c_str(), wstrProtoTag.c_str());
			}
		}

		static_cast<CSpecial_TankLorry_Head*>(m_pTankLorry_Head)->Set_Kinetic(true);
		static_cast<CSpecial_TankLorry_Head*>(m_pTankLorry_Head)->Set_ChildMatrix(m_HeadMatrix);
		static_cast<CSpecial_TankLorry_Head*>(m_pTankLorry_Head)->Sync_position(m_pTransformCom);
	}	// ~헤드 ************************************************************************** //

	{	// 트레일러 ************************************************************************** //
		wstring wstrLayer = L"Layer_Kinetic";
		string strJsonPath = "../Bin/Resources/Objects/KineticSpecial_TankLorry_Trailer.json";
		wstring wstrProtoTag = L"Prototype_GameObject_Special_TankLorry_Trailer";

		auto pProtoType = CGameInstance::GetInstance()->Find_Prototype(LEVEL_NOW, wstrProtoTag.c_str());
		if (pProtoType == nullptr)
			pProtoType = CGameInstance::GetInstance()->Find_Prototype(LEVEL_STATIC, wstrProtoTag.c_str());

		if (pProtoType == nullptr)
		{
			MSG_BOX("ProtoType is not exist");
		}
		else if (wstrLayer.empty())
		{
			MSG_BOX("Layer is empty");
		}
		else if (strJsonPath.empty() == false && CGameUtils::FileExist(s2ws(strJsonPath).c_str()) == false)
		{
			MSG_BOX("Json file is not exist");
		}
		else
		{
			if (strJsonPath.empty() == false)
			{
				Json json = CJsonStorage::GetInstance()->FindOrLoadJson(strJsonPath);
				m_pTankLorry_Trailer = CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, wstrProtoTag.c_str(), &json);
			}
			else
			{
				m_pTankLorry_Trailer = CGameInstance::GetInstance()->Clone_GameObject_Get(wstrLayer.c_str(), wstrProtoTag.c_str());
			}
		}
		static_cast<CSpecial_TankLorry_Trailer*>(m_pTankLorry_Trailer)->Set_Kinetic(true);
		static_cast<CSpecial_TankLorry_Trailer*>(m_pTankLorry_Trailer)->Set_ChildMatrix(m_TrailerMatrix);
		static_cast<CSpecial_TankLorry_Trailer*>(m_pTankLorry_Trailer)->Sync_position(m_pTransformCom);
	}	// ~트레일러 ************************************************************************** //

	{	// 오일탱크 ************************************************************************** //
		/*wstring wstrLayer = L"Layer_Kinetic";
		string strJsonPath = "../Bin/Resources/Objects/KineticSpecial_TankLorry_Tank.json";
		wstring wstrProtoTag = L"Prototype_GameObject_Special_TankLorry_Tank";

		auto pProtoType = CGameInstance::GetInstance()->Find_Prototype(LEVEL_NOW, wstrProtoTag.c_str());
		if (pProtoType == nullptr)
			pProtoType = CGameInstance::GetInstance()->Find_Prototype(LEVEL_STATIC, wstrProtoTag.c_str());

		if (pProtoType == nullptr)
		{
			MSG_BOX("ProtoType is not exist");
		}
		else if (wstrLayer.empty())
		{
			MSG_BOX("Layer is empty");
		}
		else if (strJsonPath.empty() == false && CGameUtils::FileExist(s2ws(strJsonPath).c_str()) == false)
		{
			MSG_BOX("Json file is not exist");
		}
		else
		{
			if (strJsonPath.empty() == false)
			{
				Json json = CJsonStorage::GetInstance()->FindOrLoadJson(strJsonPath);
				m_pTankLorry_Tank = CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, wstrProtoTag.c_str(), &json);
			}
			else
			{
				m_pTankLorry_Tank = CGameInstance::GetInstance()->Clone_GameObject_Get(wstrLayer.c_str(), wstrProtoTag.c_str());
			}
		}*/
	}	// ~오일탱크 ************************************************************************** //
}

CSpecial_TankLorry * CSpecial_TankLorry::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSpecial_TankLorry* pInstance = new CSpecial_TankLorry(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSpecial_TankLorry");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CSpecial_TankLorry::Clone(void * pArg)
{
	CSpecial_TankLorry* pInstance = new CSpecial_TankLorry(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSpecial_TankLorry");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSpecial_TankLorry::Free()
{
	__super::Free();

	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pTankLorry_Head))
		Safe_Release(m_pTankLorry_Head);
	if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pTankLorry_Trailer))
		Safe_Release(m_pTankLorry_Trailer);
	//if (CGameInstance::GetInstance()->Check_ObjectAlive(m_pTankLorry_Tank))
	//	Safe_Release(m_pTankLorry_Tank);
}
