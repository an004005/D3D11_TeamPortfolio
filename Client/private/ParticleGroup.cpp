#include "stdafx.h"
#include "..\public\ParticleGroup.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "CurveFloatMapImpl.h"
#include "JsonLib.h"
#include "ImguiUtils.h"
#include "GameUtils.h"
#include "Material.h"
#include "MaterialPreview.h"


CParticleGroup::CParticleGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CParticleGroup::CParticleGroup(const CParticleGroup& rhs)
	: CGameObject(rhs)
{
}

HRESULT CParticleGroup::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	if(pArg != nullptr)
	{
		Json& json = *static_cast<Json*>(pArg);

		for(auto iter : m_mapParticleSystem)
		{
			if (iter.second.second == nullptr)
				return E_FAIL;
		}
	}
	else
	{
		string ObjectTag = "1";
		string FilePath = "../Bin/Resources/VFX/ParticleSystem/";

		m_mapParticleSystem.emplace(ObjectTag, pair<string, CParticleSystem*>(FilePath, nullptr));
	}

	return S_OK;
}

void CParticleGroup::Start_ParticleWork()
{
	m_bGenerate = true;
}

void CParticleGroup::Start_NoAttach(CGameObject* pOwner, _bool trueisUpdate)
{
	if (pOwner == nullptr)
	{
		SetDelete();
		return;
	}

	m_pOwner = pOwner;
	m_bUpdate = trueisUpdate;
	m_bGenerate = true;
}

void CParticleGroup::Start_Attach(CGameObject* pOwner, string BoneName, _bool trueisUpdate)
{
	if (pOwner == nullptr)
	{
		SetDelete();
		return;
	}

	m_pOwner = pOwner;
	m_BoneName = BoneName;
	m_bUpdate = trueisUpdate;

	if (trueisUpdate == false)
	{
		_matrix	SocketMatrix = m_pOwner->GetBoneMatrix(m_BoneName) * m_pOwner->GetTransform()->Get_WorldMatrix();

		Set_Transform(SocketMatrix);
	}

	m_bGenerate = true;
}

void CParticleGroup::Start_AttachPivot(CGameObject* pOwner, _float4x4 PivotMatrix, string BoneName, _bool usepivot,	_bool trueisUpdate)
{
	if (pOwner == nullptr)
	{
		SetDelete();
		return;
	}

	m_pOwner = pOwner;
	m_BoneName = BoneName;
	m_bUpdate = trueisUpdate;
	m_bUsePivot = usepivot;
	m_PivotMatrix = PivotMatrix;

	if (trueisUpdate == false)
	{
		_matrix	SocketMatrix = m_PivotMatrix * m_pOwner->GetBoneMatrix(m_BoneName) * m_pOwner->GetTransform()->Get_WorldMatrix();

		Set_Transform(SocketMatrix);
	}

	m_bGenerate = true;
}

void CParticleGroup::Set_Transform(_matrix socket)
{
	for(auto iter : m_mapParticleSystem)
	{
		if (iter.second.second != nullptr)
			iter.second.second->GetTransform()->Set_WorldMatrix(socket);
	}
}

void CParticleGroup::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	VisibleUpdate();

	

	if(m_bUpdate == true && m_pOwner->IsDeleted() == false)
	{
		if (m_BoneName != "")
		{
			// 뼈에 붙이는 경우
			if (m_bUsePivot)
			{
				// 피봇행렬을 쓰는 경우
				_matrix	SocketMatrix = m_PivotMatrix * m_pOwner->GetBoneMatrix(m_BoneName, true) * m_pOwner->GetTransform()->Get_WorldMatrix();

				Set_Transform(SocketMatrix);
			}
			else
			{
				// 피봇행렬을 안쓰는 경우
				_matrix	SocketMatrix = m_pOwner->GetBoneMatrix(m_BoneName, true) * m_pOwner->GetTransform()->Get_WorldMatrix();

				Set_Transform(SocketMatrix);
			}
		}
		else
		{
			// 뼈에 안붙이는 경우
			Set_Transform(m_pOwner->GetTransform()->Get_WorldMatrix());
		}
	}

	if (m_bGenerate == true)
	{
		for (auto iter : m_mapParticleSystem)
		{
			if (iter.second.second != nullptr)
				iter.second.second->Tick(TimeDelta);
		}

		static _int NoGenParticle = 0;

		for (auto iter : m_mapParticleSystem)
		{
			if (iter.second.second->GetLiveParticleCnt() == 0)
			{
				NoGenParticle++;
			}
		}

		if (NoGenParticle == (_int)m_mapParticleSystem.size())
		{
			SetDelete();
		}
		NoGenParticle = 0;
	}
}

void CParticleGroup::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (m_bGenerate == true)
	{
		for (auto iter : m_mapParticleSystem)
		{
			if (iter.second.second != nullptr)
				iter.second.second->Late_Tick(TimeDelta);
		}
	}
}

void CParticleGroup::SaveToJson(Json& json)
{
	if (m_mapParticleSystem.empty() == true)
		return;

	CGameObject::SaveToJson(json);

	json["Particles"] = Json::array();

	for(auto iter : m_mapParticleSystem)
	{
		Json JsonParticle;
		JsonParticle["ObjectTag"] = iter.first;
		JsonParticle["ParticleDirectory"] = iter.second.first;
		json["Particles"].push_back(JsonParticle);
	}

	json["LifeTime"] = m_fLifeTime;
}

void CParticleGroup::LoadFromJson(const Json& json)
{
	CGameObject::LoadFromJson(json);

	// ToDO:: 컨테이너에 제대로 안들어감
	for(auto iter : json["Particles"])
	{
		string ObjectTag;
		string FilePath;

		ObjectTag = iter["ObjectTag"];
		FilePath = iter["ParticleDirectory"];

		Json Particle = CJsonStorage::GetInstance()->LoadJson_ForWork(FilePath);
		CParticleSystem* pParticleSystem; 
		pParticleSystem = dynamic_cast<CParticleSystem*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW,TEXT("ProtoVFX_ParticleSystem"), &Particle));

		m_mapParticleSystem.emplace(ObjectTag, pair<string,CParticleSystem*>(FilePath, pParticleSystem));
	}
	if(json.contains("LifeTime"))
		m_fLifeTime = json["LifeTime"];
	
}

void CParticleGroup::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();

	Load_ParticleSystem();

	ImGui::Separator();

	ImGui::BulletText("ParticleGroup List");

	static _int item_current_idx = -1;

	char** ppParticleTag = new char*[m_mapParticleSystem.size()];

	_uint i = 0;

	for(auto iter : m_mapParticleSystem)
	{
		ppParticleTag[i] = new char[MAX_PATH];
		m_ParticleTag.push_back(ppParticleTag[i]);

		sprintf_s(ppParticleTag[i], sizeof(char) * MAX_PATH, iter.first.c_str());
		i++;
	}

	// size_t Size = 

	if (m_mapParticleSystem.size() > 0)
		ImGui::ListBox("##Particle ListBox", &item_current_idx, ppParticleTag, (_int)m_mapParticleSystem.size());
	else
		ImGui::Text("[ There is no ParticleGroup Elements ]");

	if(item_current_idx != -1)
	{
		for (auto iter : m_mapParticleSystem)
		{
			if(!strcmp(iter.first.c_str(), ppParticleTag[item_current_idx]))
			{
				ImGui::BeginTabBar("ParticleGroup_Viewer");
				ImGui::Begin(iter.first.c_str());

				iter.second.second->Imgui_RenderProperty();
				iter.second.second->GetShader()->Imgui_RenderProperty();
				ImGui::End();
				ImGui::EndTabBar();
			}
		}

	}

	CImguiUtils::FileDialog_FileSelector("Save Particle Group", ".json", "../Bin/Resources/Curve/ParticleGroup/",
		[this](const string& filePath)
	{
		Json json;
		SaveToJson(json);
		std::ofstream file(filePath);
		file << json;
	});

	for (auto iter : m_ParticleTag)
	{
		Safe_Delete_Array(iter);
	}
	m_ParticleTag.clear();

	Safe_Delete_Array(ppParticleTag);

}

void CParticleGroup::Load_ParticleSystem()
{
	ImGui::Separator();
	ImGui::Separator();

	ImGui::Checkbox("Generate", &m_bGenerate);

	ImGui::NewLine();
	static _int iParticleSize = 0;
	iParticleSize = m_mapParticleSystem.size();

	ImGui::InputInt("Current ParticleSystem Count", &iParticleSize);
	
	if(ImGui::Button("Add ParticleSystem To Group"))
	{
		_uint iSize = m_mapParticleSystem.size();
		iSize += 1;

		string ObjectTag = to_string(iSize);
		string FilePath = "../Bin/Resources/VFX/ParticleSystem/";

		m_mapParticleSystem.emplace(ObjectTag, pair<string, CParticleSystem*>(FilePath, nullptr));
	}
	ImGui::Separator();
	ImGui::Separator();
	ImGui::NewLine();
	{
		static char ParticleProtoTag[MAX_PATH]{};
		ImGui::InputText("1_Particle", ParticleProtoTag, MAX_PATH);
		if (ImGui::Button("Load 1_Particle"))
		{
			if (m_mapParticleSystem.find("1")->second.second != nullptr)
				Safe_Release(m_mapParticleSystem.find("1")->second.second);

			if (m_mapParticleSystem.find("1")->second.first == "")
			{
				MSG_BOX("PLEASE WRITE THE DIRECTORY");
				return;
			}

			m_mapParticleSystem.erase("1");

			Json First_Particle = CJsonStorage::GetInstance()->LoadJson_ForWork(ParticleProtoTag);

			if (First_Particle.empty())
				MSG_BOX("Failed to Add New ParticleGroup");
			else
			{
				const string& FileName = CGameUtils::GetFileName(ParticleProtoTag);

				CParticleSystem* pParticle = nullptr;
				pParticle = dynamic_cast<CParticleSystem*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, TEXT("ProtoVFX_ParticleSystem"), &First_Particle));

				// ObjectTag							FilePath				ParticlePtr
				m_mapParticleSystem.emplace(FileName, pair<string, CParticleSystem*>(ParticleProtoTag, pParticle));

			}
		}
	}

	if(iParticleSize >= 2)
	{
		static char ParticleProtoTag[MAX_PATH]{};
		ImGui::InputText("2_Particle", ParticleProtoTag, MAX_PATH);
		if (ImGui::Button("Load 2_Particle"))
		{
			if (m_mapParticleSystem.find("2")->second.second != nullptr)
				Safe_Release(m_mapParticleSystem.find("2")->second.second);

			if (m_mapParticleSystem.find("2")->second.first == "")
			{
				MSG_BOX("PLEASE WRITE THE DIRECTORY");
				return;
			}

			m_mapParticleSystem.erase("2");

			Json Particle_Json= CJsonStorage::GetInstance()->LoadJson_ForWork(ParticleProtoTag);
			if (Particle_Json.empty())
				MSG_BOX("Failed to Add New ParticleGroup");
			else
			{
				const string& FileName = CGameUtils::GetFileName(ParticleProtoTag);

				CParticleSystem* pParticle = nullptr;
				pParticle = dynamic_cast<CParticleSystem*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, TEXT("ProtoVFX_ParticleSystem"), &Particle_Json));

				// ObjectTag							FilePath				ParticlePtr
				m_mapParticleSystem.emplace(FileName, pair<string, CParticleSystem*>(ParticleProtoTag, pParticle));

			}
		}
	}

	if (iParticleSize >= 3)
	{
		static char ParticleProtoTag[MAX_PATH]{};
		ImGui::InputText("3_Particle", ParticleProtoTag, MAX_PATH);
		if (ImGui::Button("Load 3_Particle"))
		{
			if (m_mapParticleSystem.find("3")->second.second != nullptr)
				Safe_Release(m_mapParticleSystem.find("3")->second.second);

			if (m_mapParticleSystem.find("3")->second.first == "")
			{
				MSG_BOX("PLEASE WRITE THE DIRECTORY");
				return;
			}

			m_mapParticleSystem.erase("3");

			Json Particle_Json = CJsonStorage::GetInstance()->LoadJson_ForWork(ParticleProtoTag);

			if (Particle_Json.empty())
				MSG_BOX("Failed to Add New ParticleGroup");
			else
			{
				const string& FileName = CGameUtils::GetFileName(ParticleProtoTag);

				CParticleSystem* pParticle = nullptr;
				pParticle = dynamic_cast<CParticleSystem*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, TEXT("ProtoVFX_ParticleSystem"), &Particle_Json));

				// ObjectTag							FilePath				ParticlePtr
				m_mapParticleSystem.emplace(FileName, pair<string, CParticleSystem*>(ParticleProtoTag, pParticle));
			}
		}
	}

	if (iParticleSize >= 4)
	{
		static char ParticleProtoTag[MAX_PATH]{};
		ImGui::InputText("4_Particle", ParticleProtoTag, MAX_PATH);
		if (ImGui::Button("Load 4_Particle"))
		{
			if (m_mapParticleSystem.find("4")->second.second != nullptr)
				Safe_Release(m_mapParticleSystem.find("4")->second.second);

			if (m_mapParticleSystem.find("4")->second.first == "")
			{
				MSG_BOX("PLEASE WRITE THE DIRECTORY");
				return;
			}

			m_mapParticleSystem.erase("4");

			Json Particle_Json = CJsonStorage::GetInstance()->LoadJson_ForWork(ParticleProtoTag);

			if(Particle_Json.empty())
				MSG_BOX("Failed to Add New ParticleGroup");
			else
			{
				const string& FileName = CGameUtils::GetFileName(ParticleProtoTag);

				CParticleSystem* pParticle = nullptr;
				pParticle = dynamic_cast<CParticleSystem*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, TEXT("ProtoVFX_ParticleSystem"), &Particle_Json));

				// ObjectTag							FilePath				ParticlePtr
				m_mapParticleSystem.emplace(FileName, pair<string, CParticleSystem*>(ParticleProtoTag, pParticle));
			}
		}
	}

	if (iParticleSize >= 5)
	{
		static char ParticleProtoTag[MAX_PATH]{};
		ImGui::InputText("5_Particle", ParticleProtoTag, MAX_PATH);
		if (ImGui::Button("Load 5_Particle"))
		{
			if (m_mapParticleSystem.find("5")->second.second != nullptr)
				Safe_Release(m_mapParticleSystem.find("5")->second.second);

			if (m_mapParticleSystem.find("5")->second.first == "")
			{
				MSG_BOX("PLEASE WRITE THE DIRECTORY");
				return;
			}

			m_mapParticleSystem.erase("5");

			Json Particle_Json = CJsonStorage::GetInstance()->LoadJson_ForWork(ParticleProtoTag);

			if (Particle_Json.empty())
				MSG_BOX("Failed to Add New ParticleGroup");	
			else
			{
				const string& FileName = CGameUtils::GetFileName(ParticleProtoTag);
				CParticleSystem* pParticle = nullptr;
				pParticle = dynamic_cast<CParticleSystem*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, TEXT("ProtoVFX_ParticleSystem"), &Particle_Json));

				// ObjectTag							FilePath				ParticlePtr
				m_mapParticleSystem.emplace(FileName, pair<string, CParticleSystem*>(ParticleProtoTag, pParticle));
			}
		}
	}

	if (iParticleSize >= 6)
	{
		static char ParticleProtoTag[MAX_PATH]{};
		ImGui::InputText("6_Particle", ParticleProtoTag, MAX_PATH);
		if (ImGui::Button("Load 6_Particle"))
		{
			if (m_mapParticleSystem.find("6")->second.second != nullptr)
				Safe_Release(m_mapParticleSystem.find("6")->second.second);

			if (m_mapParticleSystem.find("6")->second.first == "")
			{
				MSG_BOX("PLEASE WRITE THE DIRECTORY");
				return;
			}
			
			m_mapParticleSystem.erase("6");

			Json Particle_Json = CJsonStorage::GetInstance()->LoadJson_ForWork(ParticleProtoTag);

			if (Particle_Json.empty())
				MSG_BOX("Failed to Add New ParticleGroup"); 
			else
			{
				const string& FileName = CGameUtils::GetFileName(ParticleProtoTag);

				CParticleSystem* pParticle = nullptr;
				pParticle = dynamic_cast<CParticleSystem*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, TEXT("ProtoVFX_ParticleSystem"), &Particle_Json));

				// ObjectTag							FilePath				ParticlePtr
				m_mapParticleSystem.emplace(FileName, pair<string, CParticleSystem*>(ParticleProtoTag, pParticle));
			}
			
		}
	}
}
	
void CParticleGroup::ImGui_RenderParticleSystem(_int iSelectParticle)
{
	ImGui::BeginTabBar("ParticleGroup_Viewer");

	for(auto iter :m_mapParticleSystem)
	{
		ImGui::Begin("First_Particle");
		if (iter.second.second != nullptr)
		{
			iter.second.second->Imgui_RenderProperty();
			iter.second.second->GetShader()->Imgui_RenderProperty();
		}
		ImGui::End();
	}

	ImGui::EndTabBar();
}

void CParticleGroup::VisibleUpdate()
{
	for(auto iter : m_mapParticleSystem)
	{
		if (iter.second.second != nullptr)
		{
			iter.second.second->SetVisible(m_bVisible);
		}
	}
}


CParticleGroup* CParticleGroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CParticleGroup*		pInstance = new CParticleGroup(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CParticleGroup");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CParticleGroup::Clone(void* pArg)
{
	CParticleGroup*		pInstance = new CParticleGroup(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CParticleGroup");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CParticleGroup::Free()
{
	if (m_bCloned == true)
	{
		for (auto iter : m_mapParticleSystem)
		{
			if (iter.second.second != nullptr)
				iter.second.second->SetDelete();
		}
	}

	__super::Free();

	if (m_bCloned == true)
	{
		for (auto iter : m_mapParticleSystem)
		{
			if (iter.second.second != nullptr)
			{
				Safe_Release(iter.second.second);
				iter.second.second = nullptr;
			}
		}
	}

	m_mapParticleSystem.clear();
}
