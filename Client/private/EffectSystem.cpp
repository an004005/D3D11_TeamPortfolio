#include "stdafx.h"
#include "..\public\EffectSystem.h"

#include <JsonStorage.h>

#include "GameInstance.h"
#include "ImguiUtils.h"
#include "GameUtils.h"
#include "JsonLib.h"
#include "MathUtils.h"



CEffectSystem::CEffectSystem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffectSystem::CEffectSystem(const CEffectSystem& rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffectSystem::Initialize(void* pArg)
{
	CGameObject::Initialize(pArg);
	/* For.Com_Renderer */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)); 
	
	if (pArg == nullptr)
	{
		FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex_VFX"), TEXT("Shader"),(CComponent**)&m_pShaderCom));
		FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Buffer"),(CComponent**)&m_pBuffer));
	}
	
	if (m_ChildBuffers.empty())
		m_ChildBuffers.emplace_back(_float4x4::Identity);

	return S_OK;
}

void CEffectSystem::BeginTick()
{
}

void CEffectSystem::Tick(_double TimeDelta)
{
	
}

void CEffectSystem::Late_Tick(_double TimeDelta)
{
	if (m_bVisible)
	{
		if (m_bDecal)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DECAL, this);
		}
		else if(m_bNormal)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		}
		else
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
		}
	}
}

HRESULT CEffectSystem::Render()
{
	if (m_pShaderCom == nullptr || m_pBuffer == nullptr)
		return S_OK;

	FAILED_CHECK(SetParams());

	if(m_bModelSprite == true && m_vecModelCom.empty() == false)
	{
		Check_ModelSprite();

		if (m_pModel)
		{
			for (auto& childMatrix : m_ChildBuffers)
			{
				const _float4x4 WorldMatrix = childMatrix * m_pTransformCom->Get_WorldMatrix_f4x4();

				FAILED_CHECK(m_pShaderCom->Set_Matrix("g_WorldMatrix", &WorldMatrix));
				FAILED_CHECK(Begin());

				for (_uint i = 0; i < m_pModel->Get_NumMeshes(); ++i)
				{
					m_pModel->RenderMeshOnly(i);
				}
			}
		}
		else
		{
			for (auto& childMatrix : m_ChildBuffers)
			{
				const _float4x4 WorldMatrix = childMatrix * m_pTransformCom->Get_WorldMatrix_f4x4();

				FAILED_CHECK(m_pShaderCom->Set_Matrix("g_WorldMatrix", &WorldMatrix));
				FAILED_CHECK(Begin());

				m_pBuffer->Render();
			}
		}

	}
	else
	{
		if (m_pModel)
		{
			for (auto& childMatrix : m_ChildBuffers)
			{
				const _float4x4 WorldMatrix = childMatrix * m_pTransformCom->Get_WorldMatrix_f4x4();

				FAILED_CHECK(m_pShaderCom->Set_Matrix("g_WorldMatrix", &WorldMatrix));
				FAILED_CHECK(Begin());

				for (_uint i = 0; i < m_pModel->Get_NumMeshes(); ++i)
				{
					m_pModel->RenderMeshOnly(i);

				}
			}
		}
		else
		{
			for (auto& childMatrix : m_ChildBuffers)
			{
				const _float4x4 WorldMatrix = childMatrix * m_pTransformCom->Get_WorldMatrix_f4x4();

				FAILED_CHECK(m_pShaderCom->Set_Matrix("g_WorldMatrix", &WorldMatrix));
				FAILED_CHECK(Begin());

				m_pBuffer->Render();
			}
		}
	}

	return S_OK;
}

void CEffectSystem::Check_ModelSprite()
{
	if (m_vecModelDir.empty() == false && m_vecModelCom.empty() == false)
	{
		_uint ModelSize = (_uint)m_vecModelCom.size();

		if (ModelSize > m_tParam.Ints[0])
		{
			m_pModel = m_vecModelCom[m_tParam.Ints[0]];
		}
		else
		{
			m_pModel = m_vecModelCom[ModelSize-1];
		}
	}
}

HRESULT CEffectSystem::SetParams()
{

	if (m_bBillBoard)
	{
		switch (m_eBillBoardType)
		{
		case EBillBoardType::AXIS_ALL:
			m_pTransformCom->BillBoard(CGameInstance::GetInstance()->Get_CamPosition(), _float3::Up);
			break;
		case EBillBoardType::AXIS_LOOK:
			m_pTransformCom->LookAxisLockBillBoard(CGameInstance::GetInstance()->Get_CamPosition());
			break;
		case EBillBoardType::AXIS_UP:
			m_pTransformCom->UpAxisLockBillBoard(CGameInstance::GetInstance()->Get_CamPosition());
			break;
		case EBillBoardType::AXIS_RIGHT:
			m_pTransformCom->RightAxisLockBillBoard(CGameInstance::GetInstance()->Get_CamPosition());
			break;
		case EBillBoardType::TYPE_END:
			FALLTHROUGH
		default:
			NODEFAULT;
		}
	}

	FAILED_CHECK(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix"));
	FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ViewMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)));
	FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ProjMatrix", &CGameInstance::GetInstance()->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)));
	FAILED_CHECK(m_pShaderCom->Set_RawValue("g_vCamPosition", &CGameInstance::GetInstance()->Get_CamPosition(), sizeof(_float4)));
	
	
	FAILED_CHECK(m_pShaderCom->Set_Params(m_tParam));
	

	if (m_bDecal)
	{
		const _float4x4 ProjInv = CGameInstance::GetInstance()->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ);
		const _float4x4 ViewInv = CGameInstance::GetInstance()->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW);
		const _matrix WorldMatrix = XMLoadFloat4x4(&m_ChildBuffers[0]) * m_pTransformCom->Get_WorldMatrix();
		const _float4x4 WorldInv = XMMatrixInverse(nullptr, WorldMatrix);

		FAILED_CHECK(m_pShaderCom->Set_ShaderResourceView("g_DepthTex", CGameInstance::GetInstance()->Get_SRV(L"Target_Depth_Copy")));
		FAILED_CHECK(m_pShaderCom->Set_ShaderResourceView("g_NormalTex", CGameInstance::GetInstance()->Get_SRV(L"Target_Normal_Copy")));
		FAILED_CHECK(m_pShaderCom->Set_ShaderResourceView("g_DiffuseTex", CGameInstance::GetInstance()->Get_SRV(L"Target_Diffuse_Copy")));
		FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ProjMatrixInv", &ProjInv));
		FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ViewMatrixInv", &ViewInv));
		FAILED_CHECK(m_pShaderCom->Set_Matrix("g_WorldMatrixInv", &WorldInv));
	}
	
	if (m_bUseDepth)
	{
		FAILED_CHECK(m_pShaderCom->Set_ShaderResourceView("g_DepthTex", CGameInstance::GetInstance()->Get_SRV(L"Target_Depth_Copy")));
	}

	return S_OK;
}

HRESULT CEffectSystem::Begin()
{
	_uint iPassIndex = m_tParam.iPass;

	if (0 <= iPassIndex && iPassIndex < m_pShaderCom->GetNumPasses())
		m_tParam.iPass = iPassIndex;
	else
		m_tParam.iPass = 0;
	
	
	return m_pShaderCom->Begin(m_tParam.iPass);
}

void CEffectSystem::SaveToJson(Json& json)
{
	CShader::SaveShaderParam(m_tParam, json);

	json["bModelSprite"] = m_bModelSprite;
	json["bBillBoard"] = m_bBillBoard;
	json["BillBoardType"] = static_cast<_uint>(m_eBillBoardType);
	json["Children"] = m_ChildBuffers;
	json["bDecal"] = m_bDecal;
	json["bUseDepth"] = m_bUseDepth;
	json["bNormal"] = m_bNormal;
	json["BufferProtoTag"] = m_BufferProtoTag;
	json["ShaderProtoTag"] = m_ShaderProtoTag;
	json["ModelProtoTag"] = m_ModelProtoTag;
	json["ModelFolderDir"] = m_ModelFolderDir;

	if(m_bModelSprite == true)
	{
		json["ModelDirs"] = m_vecModelDir;
	}
}

void CEffectSystem::LoadFromJson(const Json& json)
{
	CGameObject::LoadFromJson(json);

	CShader::LoadShaderParam(m_tParam, json);

	if(json.contains("bNormal"))
	{
		m_bNormal = json["bNormal"];
	}
	else
	{
		m_bNormal = false;
	}
	if(json.contains("bModelSprite"))
	{
		m_bModelSprite = json["bModelSprite"];

		if (m_bModelSprite == true)
		{
			m_ModelFolderDir = json["ModelFolderDir"];
			m_vecModelDir = json["ModelDirs"].get<vector<string>>();

			vector<string> ModelProtoTags = json["ModelDirs"];
			for(auto iter :ModelProtoTags)
			{
				if(iter.empty())
				{
					m_vecModelCom.push_back({nullptr});
				}
				else
				{
					m_vecModelCom.push_back({dynamic_cast<CModel*>(CGameInstance::GetInstance()->Clone_Component(CGameUtils::s2ws(iter).c_str()))});
				}
			}
			m_pModel = m_vecModelCom[0];

		}
	}
	else
	{
		m_bModelSprite = false;
	}

	m_bBillBoard = json["bBillBoard"];
	m_eBillBoardType = json["BillBoardType"];
	m_ChildBuffers = json["Children"];
	m_BufferProtoTag = json["BufferProtoTag"];
	m_ShaderProtoTag = json["ShaderProtoTag"];
	
	if (json.contains("ModelProtoTag"))
		m_ModelProtoTag = json["ModelProtoTag"];

	if (json.contains("bDecal"))
		m_bDecal = json["bDecal"];
	if (json.contains("bUseDepth"))
		m_bUseDepth = json["bUseDepth"];

	FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_BufferProtoTag).c_str(), TEXT("Buffer"), (CComponent**)&m_pBuffer));
	FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_ShaderProtoTag).c_str(), TEXT("Shader"), (CComponent**)&m_pShaderCom));

	if (m_ModelProtoTag.empty() == false)
	{
		CModel::s_DefaultPivot = _float4x4::CreateScale({ 0.01f, 0.01f, 0.01f });
		FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_ModelProtoTag).c_str(), TEXT("Model"), (CComponent**)&m_pModel));
		CModel::s_DefaultPivot = _float4x4::CreateScale({ 0.01f, 0.01f, 0.01f }) *_float4x4::CreateRotationY(XMConvertToRadians(-180.f));
	}
}

void CEffectSystem::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();
	
	ImGui::Separator();

	
	CShader::Imgui_RenderShaderParams(m_tParam);

	if(ImGui::Button("Set_OriginColor"))
	{
		m_vOriginColor = m_tParam.Float4s[0];
	}

	ImGui::Separator();

	if (ImGui::Button("Add Child Buffer"))
	{
		m_ChildBuffers.emplace_back(_float4x4{});
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Child Buffer"))
	{
		if (!m_ChildBuffers.empty())
			m_ChildBuffers.pop_back();
	}
	
	static _int iSelectBuffer = 0;
	static _bool bUseGuizmo = false;
	ImGui::Text("Current Child Nums : %d", static_cast<_int>(m_ChildBuffers.size()));
	ImGui::InputInt("Select Child Buffer", &iSelectBuffer);
	if (iSelectBuffer < 0 || iSelectBuffer >= static_cast<_int>(m_ChildBuffers.size()))
		iSelectBuffer = 0;
	ImGui::Checkbox("Use Child Guizmo", &bUseGuizmo);
	if (bUseGuizmo && m_ChildBuffers.empty() == false)
	{
		static GUIZMO_INFO tInfo;
		CImguiUtils::Render_Guizmo(&m_ChildBuffers[iSelectBuffer], tInfo, true, true);
	}
	
	ImGui::Separator();
	{
		char BufferProtoTag[MAX_PATH];
		strcpy(BufferProtoTag, m_BufferProtoTag.c_str());
		ImGui::InputText("Buffer ProtoTag", BufferProtoTag, MAX_PATH);
		m_BufferProtoTag = BufferProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("ReCreate Buffer"))
		{
			Safe_Release(m_pBuffer);
			Delete_Component(TEXT("Buffer"));
			FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_BufferProtoTag).c_str(), TEXT("Buffer"), (CComponent**)&m_pBuffer));
		}
	}
	// {
	// 	char BufferProtoTag[MAX_PATH];
	// 	strcpy(BufferProtoTag, m_BufferProtoTag.c_str());
	// 	ImGui::InputText("InstanceBuffer ProtoTag", BufferProtoTag, MAX_PATH);
	// 	m_BufferProtoTag = BufferProtoTag;
	// 	ImGui::SameLine();
	// 	if (ImGui::Button("ReCreate Buffer"))
	// 	{
	// 		Safe_Release(m_pBuffer);
	// 		Delete_Component(TEXT("Instance_Buffer"));
	// 		FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_BufferProtoTag).c_str(), TEXT("Buffer"), (CComponent**)&m_pBuffer));
	// 	}
	// }
	{
		char ShaderProtoTag[MAX_PATH];
		strcpy(ShaderProtoTag, m_ShaderProtoTag.c_str());
		ImGui::InputText("Shader ProtoTag", ShaderProtoTag, MAX_PATH);
		m_ShaderProtoTag = ShaderProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("ReCreate Shader"))
		{
			Safe_Release(m_pShaderCom);
			Delete_Component(TEXT("Shader"));
			FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_ShaderProtoTag).c_str(), TEXT("Shader"), (CComponent**)&m_pShaderCom));
		}
	}
	{
		char ModelProtoTag[MAX_PATH];
		strcpy(ModelProtoTag, m_ModelProtoTag.c_str());
		ImGui::InputText("Model ProtoTag", ModelProtoTag, MAX_PATH);
		m_ModelProtoTag = ModelProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("ReCreate Model"))
		{
			Safe_Release(m_pModel);
			Delete_Component(TEXT("Model"));
			Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_ModelProtoTag).c_str(), TEXT("Model"), 	(CComponent**)&m_pModel);
		}
	}

	ImGui::Checkbox("Model Sprite", &m_bModelSprite);

	if(m_bModelSprite == true)
	{
		char ModelFolderDir[MAX_PATH];
		strcpy(ModelFolderDir, m_ModelFolderDir.c_str());
		ImGui::InputText("Model Folder Directory", ModelFolderDir, MAX_PATH);
		m_ModelFolderDir = ModelFolderDir;
		ImGui::SameLine();
		if (ImGui::Button("Load Models"))
		{
			Safe_Release(m_pModel);
			Delete_Component(TEXT("Model"));

			CGameUtils::ListFiles(m_ModelFolderDir, [this](const string& pEffectPath)
			{
				m_vecModelDir.push_back(pEffectPath);
			});

			if(m_vecModelDir.empty())
			{
				MSG_BOX("Empty Folder");
				return;
			}
			else
			{
				_uint ModelDirSize = (_uint)m_vecModelDir.size();

				for(_uint i =0; i < ModelDirSize; ++i)
				{
					m_vecModelCom.push_back({ dynamic_cast<CModel*>(CGameInstance::GetInstance()->Clone_Component(CGameUtils::s2ws(m_vecModelDir[i]).c_str())) });
				}
				m_pModel = m_vecModelCom[0];
			}
		}
	}

	ImGui::Checkbox("NormalTex", &m_bNormal);

	ImGui::Checkbox("BillBoard", &m_bBillBoard);
	if (m_bBillBoard)
	{
		ImGui::Text("BillBoardType");
		if (ImGui::RadioButton("All", m_eBillBoardType == EBillBoardType::AXIS_ALL))
			m_eBillBoardType = EBillBoardType::AXIS_ALL;
		if (ImGui::RadioButton("Look", m_eBillBoardType == EBillBoardType::AXIS_LOOK))
			m_eBillBoardType = EBillBoardType::AXIS_LOOK;
		if (ImGui::RadioButton("Up", m_eBillBoardType == EBillBoardType::AXIS_UP))
			m_eBillBoardType = EBillBoardType::AXIS_UP;
		if (ImGui::RadioButton("Right", m_eBillBoardType == EBillBoardType::AXIS_RIGHT))
			m_eBillBoardType = EBillBoardType::AXIS_RIGHT;
	}
	
	ImGui::Checkbox("Use DecalParams", &m_bDecal);
	ImGui::Checkbox("Use Depth Tex", &m_bUseDepth);
	
	ImGui::Separator();
	CImguiUtils::FileDialog_FileSelector("Save This Effect", ".json", "../Bin/Resources/VFX/EffectSystem/", [this](const string& filePath)
	{
		Json json;
		SaveToJson(json);
		std::ofstream file(filePath);
		file << json;
	});

	
	
}

void CEffectSystem::Tick_Scale_All(_float fValue)
{
	fValue *= 2.f;
	_float3 Scale = m_pTransformCom->Get_Scaled();

	m_pTransformCom->Set_Scaled(_float3(fValue, fValue, fValue));
}

void CEffectSystem::Tick_Scale_Y(_float fValue)
{
	fValue *= 2.f;
	_float3 Scale = m_pTransformCom->Get_Scaled();

	m_pTransformCom->Set_Scaled(_float3(Scale.x, fValue, Scale.x));
}

void CEffectSystem::Tick_Scale_X(_float fValue)
{
	fValue *= 2.f;
	_float3 Scale = m_pTransformCom->Get_Scaled();

	m_pTransformCom->Set_Scaled(_float3(fValue, Scale.y, Scale.z));
}

void CEffectSystem::Tick_Floats_0(_float fValue)
{
	m_tParam.Floats[0] = fValue;
}

void CEffectSystem::Tick_Floats_1(_float fValue)
{
	m_tParam.Floats[1] = fValue;
}

void CEffectSystem::Tick_Floats_2(_float fValue)
{
	m_tParam.Floats[2] = fValue;
}

void CEffectSystem::Tick_ColorChange(_float fValue)
{
	if (m_tParam.Float4s.size() < 2)
		return;

	_float4 vOriginColor = m_vOriginColor;
	_float4 vChangeColor = m_tParam.Float4s[1];

	_float4 vOutColor = vOriginColor * (1.f - fValue);
	_float4 vInColor = vChangeColor * (fValue);

	// _float4 vResultColor = CMathUtils::Clamp(vOutColor + vInColor, _float4::Zero, _float4::One) ;
	
	m_tParam.Float4s[0] = vOutColor  + vInColor;
}

void CEffectSystem::Tick_Floats_3(_float fValue)
{
	m_tParam.Floats[3] = fValue;
}

void CEffectSystem::Tick_Floats_4(_float fValue)
{
	m_tParam.Floats[4] = fValue;
}

void CEffectSystem::Tick_Floats_5(_float fValue)
{
	m_tParam.Floats[5] = fValue;
}

void CEffectSystem::Tick_Floats_6(_float fValue)
{
	m_tParam.Floats[6] = fValue;
}

void CEffectSystem::Tick_Floats_7(_float fValue)
{
	m_tParam.Floats[7] = fValue;
}

void CEffectSystem::Tick_IntroTime(_float fValue)
{
	m_tParam.Floats[8] = fValue;
}

void CEffectSystem::Tick_OutroTime(_float fValue)
{
	m_tParam.Floats[9] = fValue;
}

void CEffectSystem::Tick_Ints_0(_float fValue)
{
	m_tParam.Ints[0] = (_int)fValue;
	// int a = 3;
	// IM_LOG("%f", fValue);
}


CEffectSystem* CEffectSystem::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffectSystem*		pInstance = new CEffectSystem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffectSystem");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffectSystem::Clone(void* pArg)
{
	CEffectSystem*		pInstance = new CEffectSystem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffectSystem");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffectSystem::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pModel);

	_uint ModelSize = (_uint)m_vecModelCom.size();

	for(_uint i =0;i<ModelSize; ++i)
	{
		Safe_Release(m_vecModelCom[i]);
	}
	m_vecModelCom.clear();

	for (auto e : m_tParam.Textures)
		Safe_Release(e.first);
}
