#include "stdafx.h"
#include "..\public\EffectSystem.h"
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
	// m_fCurModelChangeTime += (_float)TimeDelta;

	// if (m_fCurModelChangeTime >= m_fModelChangeTime &&m_bModelChange == false)
	// {
	// 	m_fCurModelChangeTime = 0.f;
	// 	m_bModelChange = true;
	// }

	// if(m_pBuffer->Check_IsInstance() == true)
	// 	dynamic_cast<CVIBuffer_Mesh_Instancing*>(m_pBuffer)->Tick(TimeDelta);
}

void CEffectSystem::Late_Tick(_double TimeDelta)
{
	if (m_bVisible)
	{
		if (m_bDecal)
		{
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DECAL, this);
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

				// if(m_fCurModelChangeTime >= m_fModelChangeTime)
				// {
				// 	m_fCurModelChangeTime = 0.f;
				// 	++i;
				// }
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

	return S_OK;
}

HRESULT CEffectSystem::SetParams()
{
	// if (auto pBillborad = dynamic_cast<CBillBoardTest*>(m_pBuffer))
	// {
	// 	pBillborad->Tick(g_fTimeDelta);
	// }
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
	// if(m_tParam.iPass > m_pShaderCom->GetNumPasses())
	// {
	// 	m_tParam.iPass = 0;
	// }
	// if (nullptr == m_pContext ||
	// 	m_tParam.iPass >= m_pShaderCom->GetNumPasses())
	// 	return E_FAIL;
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
	
	json["bBillBoard"] = m_bBillBoard;
	json["BillBoardType"] = static_cast<_uint>(m_eBillBoardType);
	json["Children"] = m_ChildBuffers;
	json["bDecal"] = m_bDecal;
	json["bUseDepth"] = m_bUseDepth;
	
	json["BufferProtoTag"] = m_BufferProtoTag;
	json["ShaderProtoTag"] = m_ShaderProtoTag;
	json["ModelProtoTag"] = m_ModelProtoTag;
	
}

void CEffectSystem::LoadFromJson(const Json& json)
{
	CGameObject::LoadFromJson(json);

	CShader::LoadShaderParam(m_tParam, json);

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
		FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_ModelProtoTag).c_str(), TEXT("Model"), (CComponent**)&m_pModel));
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
			FAILED_CHECK(Add_Component(LEVEL_NOW, CGameUtils::s2ws(m_ModelProtoTag).c_str(), TEXT("Model"), 	(CComponent**)&m_pModel));
		}
	}

	

	// if (auto pBillBoard = dynamic_cast<CBillBoardTest*>(m_pBuffer))
	// {
	// 	ImGui::InputFloat("BillBoardLife", &pBillBoard->m_fLife);
	// }
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

void CEffectSystem::Tick_Scale(_float fValue)
{
	fValue *= 2.f;

	m_pTransformCom->Set_Scaled(_float3(fValue, fValue, fValue));
}

void CEffectSystem::Tick_IntroDissolve(_float fValue)
{
	m_tParam.Floats[0] = fValue;
}

void CEffectSystem::Tick_OutroDissolve(_float fValue)
{
	m_tParam.Floats[0] = fValue;
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

void CEffectSystem::Tick_EmissiveChange(_float fValue)
{
	m_tParam.Floats[0] = fValue;
}

void CEffectSystem::Tick_IntroTime(_float fValue)
{
	m_tParam.Floats[0] = fValue;
}

void CEffectSystem::Tick_OutroTime(_float fValue)
{
	m_tParam.Floats[0] = fValue;
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

	for (auto e : m_tParam.Textures)
		Safe_Release(e.first);
}
