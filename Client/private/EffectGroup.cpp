#include "stdafx.h"
#include "..\public\EffectGroup.h"
#include "GameInstance.h"
#include "JsonStorage.h"
#include "CurveFloatMapImpl.h"
#include "JsonLib.h"
#include "ImguiUtils.h"
#include "EffectSystem.h"
#include "GameUtils.h"
#include "Material.h"
#include "MaterialPreview.h"

CEffectGroup::CEffectGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffectGroup::CEffectGroup(const CEffectGroup& rhs)
	: CGameObject(rhs)
{

}

HRESULT CEffectGroup::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	if(pArg != nullptr)
	{
		Json& json = *static_cast<Json*>(pArg);

		if(m_First_EffectDirectory != "")
		{
			Json First_Effect = CJsonStorage::GetInstance()->FindOrLoadJson(m_First_EffectDirectory);
			m_pFirst_EffectSystem = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectGroup", L"ProtoVFX_EffectSystem", &First_Effect));
			Safe_AddRef(m_pFirst_EffectSystem);
			
			const string& strFileName =  CGameUtils::GetFileName(m_First_EffectDirectory);
			m_mapEffectSystemTag.emplace(strFileName, m_pFirst_EffectSystem);


			if (json.contains("FirstEffect_Curves"))
			{
				for (auto curveJson : json["FirstEffect_Curves"])
				{
					LoadAndSetCurve_First(&curveJson);
				}
			}

		}
		if (m_Second_EffectDirectory != "")
		{
			Json Second_Effect = CJsonStorage::GetInstance()->FindOrLoadJson(m_Second_EffectDirectory);
			m_pSecond_EffectSystem = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectGroup", L"ProtoVFX_EffectSystem", &Second_Effect));
			const string& strFileName = CGameUtils::GetFileName(m_Second_EffectDirectory);
			m_mapEffectSystemTag.emplace(strFileName, m_pSecond_EffectSystem);
			Safe_AddRef(m_pSecond_EffectSystem);
			
			if (json.contains("SecondEffect_Curves"))
			{
				for (auto curveJson : json["SecondEffect_Curves"])
				{
					LoadAndSetCurve_Second(&curveJson);
				}
			}
		}
		if (m_Third_EffectDirectory != "")
		{
			Json Third_Effect = CJsonStorage::GetInstance()->FindOrLoadJson(m_Third_EffectDirectory);
			m_pThird_EffectSystem = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectGroup", L"ProtoVFX_EffectSystem", &Third_Effect));
			const string& strFileName = CGameUtils::GetFileName(m_Third_EffectDirectory);
			m_mapEffectSystemTag.emplace(strFileName, m_pThird_EffectSystem);
			Safe_AddRef(m_pThird_EffectSystem);
			
			if (json.contains("ThirdEffect_Curves"))
			{
				for (auto curveJson : json["ThirdEffect_Curves"])
				{
					LoadAndSetCurve_Third(&curveJson);
				}
			}
		}
		if (m_Fourth_EffectDirectory != "")
		{
			Json Fourth_Effect = CJsonStorage::GetInstance()->FindOrLoadJson(m_Fourth_EffectDirectory);
			m_pFourth_EffectSystem = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectGroup", L"ProtoVFX_EffectSystem", &Fourth_Effect));
			const string& strFileName = CGameUtils::GetFileName(m_Fourth_EffectDirectory);
			m_mapEffectSystemTag.emplace(strFileName,m_pFourth_EffectSystem);
			Safe_AddRef(m_pFourth_EffectSystem);
			if (json.contains("FourthEffect_Curves"))
			{
				for (auto curveJson : json["FourthEffect_Curves"])
				{
					LoadAndSetCurve_Fourth(&curveJson);
				}
			}
		}
		if (m_Fifth_EffectDirectory != "")
		{
			Json Fifth_Effect = CJsonStorage::GetInstance()->FindOrLoadJson(m_Fifth_EffectDirectory);
			m_pFifth_EffectSystem = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectGroup", L"ProtoVFX_EffectSystem", &Fifth_Effect));
			const string& strFileName = CGameUtils::GetFileName(m_Fifth_EffectDirectory);
			m_mapEffectSystemTag.emplace(strFileName, m_pFifth_EffectSystem);
			Safe_AddRef(m_pFifth_EffectSystem);

			if (json.contains("FifthEffect_Curves"))
			{
				for (auto curveJson : json["FifthEffect_Curves"])
				{
					LoadAndSetCurve_Fifth(&curveJson);
				}
			}
		} 

		m_Timeline.SetTimelineLength((_double)m_fEndTime);

		//m_Timeline.SetFinishFunction((CGameObject*)this, &CEffectGroup::SetDelete);
		// m_Timeline.SetFinishFunction([this]
		// {
		// 	SetDelete();
		// });

		 if (m_iSelectFinishFunc == 0)
		 {
		 	m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::PlayFromStart);
		 }
		 else if (m_iSelectFinishFunc == 1)
		 {
		 	m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Reset);
		 }
		 else if (m_iSelectFinishFunc == 2)
		 {
		 	m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Stop);
		 }
		 else if (m_iSelectFinishFunc == 3)
		 {
		 	m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Reverse);
		 }

//		 m_Timeline.PlayFromStart();

	}
	else
	{
		m_Timeline.SetTimelineLength(3.0);
		m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Reset);
	}

	return S_OK;
}

void CEffectGroup::Start_NoAttach(CGameObject* pOwner, _bool trueisUpdate)
{
	if (pOwner == nullptr)
	{
		SetDelete();
		return;
	}

	if (trueisUpdate == false)
	{
		_matrix	SocketMatrix = m_pOwner->GetTransform()->Get_WorldMatrix();

		Set_Transform(SocketMatrix);
	}

	m_pOwner = pOwner;
	m_bUpdate = trueisUpdate;
	m_Timeline.PlayFromStart();
}

void CEffectGroup::Start_Attach(CGameObject* pOwner, string BoneName, _bool trueisUpdate)
{
	if (pOwner == nullptr)
	{
		SetDelete();
		return;
	}

	m_pOwner = pOwner;
	m_BoneName = BoneName;
	m_bUpdate = trueisUpdate;

	if(m_bUpdate == false)
	{
		_matrix	SocketMatrix = m_pOwner->GetBoneMatrix(m_BoneName) * m_pOwner->GetTransform()->Get_WorldMatrix();

		Set_Transform(SocketMatrix);
	}

	m_Timeline.PlayFromStart();
}

void CEffectGroup::Start_AttachPivot(CGameObject* pOwner, _float4x4 PivotMatrix, string BoneName, _bool usepivot, _bool trueisUpdate)
{
	if (pOwner == nullptr)
	{
		SetDelete();
		return;
	}

	m_pOwner = pOwner;
	m_bUpdate = trueisUpdate;
	m_BoneName = BoneName;
	m_bUsePivot = usepivot;
	m_PivotMatrix = PivotMatrix;

	if (trueisUpdate == false)
	{
		_matrix	SocketMatrix = m_pOwner->GetBoneMatrix(m_BoneName, true) * m_pOwner->GetTransform()->Get_WorldMatrix();

		Set_Transform(SocketMatrix);
	}

	m_Timeline.PlayFromStart();
}

void CEffectGroup::Start_AttachPosition(CGameObject * pOwner, _float4 vPosition, _float4 vDirection, _bool trueisUpdate)
{
	if (pOwner == nullptr)
	{
		SetDelete();
		return;
	}

	m_pOwner = pOwner;
	m_bUpdate = trueisUpdate;

	if (trueisUpdate == false)
	{
		_matrix	SocketMatrix = XMMatrixTranslation(vPosition.x, vPosition.y, vPosition.z);

		_vector		vUp = XMVector3Normalize(vDirection);
		_vector		vRight = XMVector3Normalize(XMVector3Cross(vUp, XMVectorSet(0.f, 0.f, 1.f, 0.f)));
		_vector		vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));

		SocketMatrix.r[0] = vRight;
		SocketMatrix.r[1] = vUp;
		SocketMatrix.r[2] = vLook;

		Set_Transform(SocketMatrix);
	}

	m_Timeline.PlayFromStart();
}

void CEffectGroup::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);
	m_Timeline.Tick(TimeDelta);
	VisibleUpdate();
	

	if(m_bUpdate == true && m_pOwner->IsDeleted() == false)
	{
		if(m_BoneName != "")
		{
			// 뼈에 붙이는 경우
			if(m_bUsePivot)
			{
				// 피봇행렬을 쓰는 경우
				_matrix	SocketMatrix = m_PivotMatrix * m_pOwner->GetBoneMatrix(m_BoneName) * m_pOwner->GetTransform()->Get_WorldMatrix() ;

				Set_Transform(SocketMatrix);
			}
			else
			{
				// 피봇행렬을 안쓰는 경우
				_matrix	SocketMatrix = m_pOwner->GetBoneMatrix(m_BoneName) * m_pOwner->GetTransform()->Get_WorldMatrix();

				Set_Transform(SocketMatrix);
			}
		}
		else
		{
			// 뼈에 안붙이는 경우
			Set_Transform(m_pOwner->GetTransform()->Get_WorldMatrix());
		}
	}
	
}

void CEffectGroup::Imgui_RenderProperty()
{
	CGameObject::Imgui_RenderProperty();

	Load_EffectSystem();

	ImGui::Separator();

	ImGui::BulletText("Timeline_Options");
	ImGui::SliderFloat("Set_EndTime", &m_fEndTime, 0.f, 30.f);
	ImGui::SameLine();
	if(ImGui::Button("Set_End"))
	{
		m_Timeline.SetTimelineLength(m_fEndTime);
	}


	// m_Timeline.SetTimelineLength(3.0);

	ImGui::Combo("Finish_Function", &m_iSelectFinishFunc, m_szFuncName, FUNC_END);
	ImGui::SameLine();
	if(ImGui::Button("Set_Fin-Function"))
	{
		if(m_iSelectFinishFunc == 0)
		{
			m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::PlayFromStart);
		}
		else if (m_iSelectFinishFunc == 1)
		{
			m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Reset);
		}
		else if (m_iSelectFinishFunc == 2)
		{
			m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Stop);
		}
		else if (m_iSelectFinishFunc == 3)
		{
			m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Reverse);
		}
		// 삭제 또는 비활성화 만들기
	}


	// if(ImGui::Button("PlayFromStart"))
	// {
	// 	m_Timeline.PlayFromStart();
	// }

	m_Timeline.Imgui_RenderEditor();

	ImGui::Separator();
	ImGui::Separator();

	static _int item_current_idx = -1;

	ImGui::BulletText("EffectGroup List");

	char** ppEffectGroupTag = new char*[m_mapEffectSystemTag.size()];

	_uint i = 0;
	
	for(auto iter : m_mapEffectSystemTag)
	{
		ppEffectGroupTag[i] = new char[MAX_PATH];
		ppEffectTag.push_back(ppEffectGroupTag[i]);

		sprintf_s(ppEffectGroupTag[i], sizeof(char)*MAX_PATH, iter.first.c_str());
		i++;
	}

	if (m_mapEffectSystemTag.size() > 0)
		ImGui::ListBox("##EffectGroup ListBox", &item_current_idx, ppEffectGroupTag, (_int)m_mapEffectSystemTag.size());
	else
		ImGui::Text("    [ There is no EffectGroup Elements ]");

	if (CGameInstance::GetInstance()->KeyDown(DIK_ESCAPE))
	{
		item_current_idx = -1;
	}

	// if(ImGui::BeginListBox("##EffectGroup Box", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
	// {
	// 	for(_int n= 0; n<m_mapEffectSystemTag.size(); n++)
	// 	{
	// 		const _bool is_selected = (item_current_idx == n);
	//
	// 		if (ImGui::Selectable(m_mapEffectSystemTag[n].first.c_str(), is_selected))
	// 		{
	// 			item_current_idx = n;
	// 		}
	//
	// 		if (is_selected)
	// 			ImGui::SetItemDefaultFocus();
	//
	// 		if (CGameInstance::GetInstance()->KeyDown(DIK_ESCAPE))
	// 		{
	// 			item_current_idx = -1;
	// 		}
	// 			
	// 	}
	// 	ImGui::EndListBox();
	// }

	if (item_current_idx != -1)
	{
		static _int	iSelectCurve = -1;

		IMGUI_LEFT_LABEL(ImGui::Combo, "Target Curve", &iSelectCurve, m_szCurveTag, CURVE_END);
		ImGui::SameLine();

		if (ImGui::Button("Add Target Curve"))
		{
			if (iSelectCurve == -1 || iSelectCurve > CURVE_END)
			{
				MSG_BOX("Wrong Curve Number");
				return;
			}

			if (item_current_idx == -1 || item_current_idx > (_int)m_mapEffectSystemTag.size())
			{
				MSG_BOX("Wrong EffectTag");
				return;
			}

			Add_TargetCurve(item_current_idx, m_szCurveTag[iSelectCurve]);

			iSelectCurve = -1;
		}

		if (item_current_idx == 0)
		{
			static char szCurveTag[MAX_PATH] = "";
			IMGUI_LEFT_LABEL(ImGui::InputTextWithHint, "Add FirstEffect's Curve", " Input Curve Tag And Press Add Button", szCurveTag, MAX_PATH);
			ImGui::SameLine();

			if (ImGui::Button("Add") && strcmp(szCurveTag, ""))
			{
				AddEmptyCurve_ForFirst(szCurveTag);
				ZeroMemory(szCurveTag, MAX_PATH);
			}

			string CurveOwnerName = ppEffectGroupTag[0];

			ImGui::BulletText("[ %s ]'s Curve List", CurveOwnerName.c_str());
			static const char* pCurName = "";
			if (ImGui::BeginListBox("##FirstEffect Curve List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (auto pCurve : m_FirstEffect_Curves)
				{
					const bool bSelected = strcmp(pCurve.first, pCurName) == 0;
					if (bSelected)
						ImGui::SetItemDefaultFocus();
					if (ImGui::Selectable(pCurve.first, bSelected))
						pCurName = pCurve.first;
				}
				ImGui::EndListBox();
			}

			auto itr = m_FirstEffect_Curves.find(pCurName);

			if (itr != m_FirstEffect_Curves.end())
			{
				itr->second->Imgui_RenderEditor();
			}

			

			Imgui_RenderEffectSource(item_current_idx);

		}
		else if (item_current_idx == 1)
		{
			static char szCurveTag[MAX_PATH] = "";
			IMGUI_LEFT_LABEL(ImGui::InputTextWithHint, "Add SecondEffect's Curve", " Input Curve Tag And Press Add Button", szCurveTag, MAX_PATH);
			ImGui::SameLine();

			if (ImGui::Button("Add") && strcmp(szCurveTag, ""))
			{
				AddEmptyCurve_ForSecond(szCurveTag);
				ZeroMemory(szCurveTag, MAX_PATH);
			}

			string CurveOwnerName = ppEffectGroupTag[1];
			ImGui::BulletText("[ %s ]'s Curve List", CurveOwnerName.c_str());
			static const char* pCurName = "";
			if (ImGui::BeginListBox("##SecondEffect Curve List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (auto pCurve : m_SecondEffect_Curves)
				{
					const bool bSelected = strcmp(pCurve.first, pCurName) == 0;
					if (bSelected)
						ImGui::SetItemDefaultFocus();
					if (ImGui::Selectable(pCurve.first, bSelected))
						pCurName = pCurve.first;
				}
				ImGui::EndListBox();
			}

			auto itr = m_SecondEffect_Curves.find(pCurName);

			if (itr != m_SecondEffect_Curves.end())
			{
				itr->second->Imgui_RenderEditor();
			}

			Imgui_RenderEffectSource(item_current_idx);
		}
		else if (item_current_idx == 2)
		{
			static char szCurveTag[MAX_PATH] = "";
			IMGUI_LEFT_LABEL(ImGui::InputTextWithHint, "Add ThirdEffect's Curve", " Input Curve Tag And Press Add Button", szCurveTag, MAX_PATH);
			ImGui::SameLine();

			if (ImGui::Button("Add") && strcmp(szCurveTag, ""))
			{
				AddEmptyCurve_ForThird(szCurveTag);
				ZeroMemory(szCurveTag, MAX_PATH);
			}

			string CurveOwnerName = ppEffectGroupTag[2];
			ImGui::BulletText("[ %s ]'s Curve List", CurveOwnerName.c_str());
			static const char* pCurName = "";
			if (ImGui::BeginListBox("##ThirdEffect Curve List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (auto pCurve : m_ThirdEffect_Curves)
				{
					const bool bSelected = strcmp(pCurve.first, pCurName) == 0;
					if (bSelected)
						ImGui::SetItemDefaultFocus();
					if (ImGui::Selectable(pCurve.first, bSelected))
						pCurName = pCurve.first;
				}
				ImGui::EndListBox();
			}

			auto itr = m_ThirdEffect_Curves.find(pCurName);

			if (itr != m_ThirdEffect_Curves.end())
			{
				itr->second->Imgui_RenderEditor();
			}

			Imgui_RenderEffectSource(item_current_idx);

		}
		else if (item_current_idx == 3)
		{
			static char szCurveTag[MAX_PATH] = "";
			IMGUI_LEFT_LABEL(ImGui::InputTextWithHint, "Add FourthEffect's Curve", " Input Curve Tag And Press Add Button", szCurveTag, MAX_PATH);
			ImGui::SameLine();

			if (ImGui::Button("Add") && strcmp(szCurveTag, ""))
			{
				AddEmptyCurve_ForFourth(szCurveTag);
				ZeroMemory(szCurveTag, MAX_PATH);
			}

			string CurveOwnerName = ppEffectGroupTag[3];
			ImGui::BulletText("[ %s ]'s Curve List", CurveOwnerName.c_str());
			static const char* pCurName = "";
			if (ImGui::BeginListBox("##FourthEffect Curve List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (auto pCurve : m_FourthEffect_Curves)
				{
					const bool bSelected = strcmp(pCurve.first, pCurName) == 0;
					if (bSelected)
						ImGui::SetItemDefaultFocus();
					if (ImGui::Selectable(pCurve.first, bSelected))
						pCurName = pCurve.first;
				}
				ImGui::EndListBox();
			}

			auto itr = m_FourthEffect_Curves.find(pCurName);

			if (itr != m_FourthEffect_Curves.end())
			{
				itr->second->Imgui_RenderEditor();
			}

			Imgui_RenderEffectSource(item_current_idx);
		}
		else if (item_current_idx == 4)
		{
			static char szCurveTag[MAX_PATH] = "";
			IMGUI_LEFT_LABEL(ImGui::InputTextWithHint, "Add FifthEffect's Curve", " Input Curve Tag And Press Add Button", szCurveTag, MAX_PATH);
			ImGui::SameLine();

			if (ImGui::Button("Add") && strcmp(szCurveTag, ""))
			{
				AddEmptyCurve_ForFifth(szCurveTag);
				ZeroMemory(szCurveTag, MAX_PATH);
			}

			string CurveOwnerName = ppEffectGroupTag[4];
			ImGui::BulletText("[ %s ]'s Curve List", CurveOwnerName.c_str());
			static const char* pCurName = "";
			if (ImGui::BeginListBox("##FifthEffect Curve List", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
			{
				for (auto pCurve : m_FifthEffect_Curves)
				{
					const bool bSelected = strcmp(pCurve.first, pCurName) == 0;
					if (bSelected)
						ImGui::SetItemDefaultFocus();
					if (ImGui::Selectable(pCurve.first, bSelected))
						pCurName = pCurve.first;
				}
				ImGui::EndListBox();
			}

			auto itr = m_FifthEffect_Curves.find(pCurName);

			if (itr != m_FifthEffect_Curves.end())
			{
				itr->second->Imgui_RenderEditor();
			}

			Imgui_RenderEffectSource(item_current_idx);

		}
		else
		{
			ImGui::Separator();
			ImGui::Separator();

			ImGui::Text("    [ Please Select EffectGroup Element ] ");
		}

		
		m_Timeline.ImGui_RenderTimelineEvent();
	}
	// 커브 추가하는 곳


	ImGui::Separator();
	ImGui::Separator();


	CImguiUtils::FileDialog_FileSelector("Save Curves And Group", ".json", "../Bin/Resources/Curve/",
		[this](const string& filePath)
	{
		Json json;
		SaveToJson(json);
		std::ofstream file(filePath);
		file << json;
	});	

	// Imgui_RenderEffectSource();

	for (auto iter : ppEffectTag)
	{
		Safe_Delete_Array(iter);
	}
	ppEffectTag.clear();

	Safe_Delete_Array(ppEffectGroupTag);
		
}

void CEffectGroup::SaveToJson(Json& json)
{
	CGameObject::SaveToJson(json);

	json["Curves"] = Json::array();

	if(m_pFirst_EffectSystem != nullptr)
	{
		json["First_Directory"] = m_First_EffectDirectory;

		for (auto pCurve : m_FirstEffect_Curves)
		{
			Json curveJson;
			pCurve.second->SaveToJson(curveJson);
			json["FirstEffect_Curves"].push_back(curveJson);
		}
	}
	else
		json["First_Directory"] = "";

	if(m_pSecond_EffectSystem != nullptr)
	{
		json["Second_Directory"] = m_Second_EffectDirectory;

		for (auto pCurve : m_SecondEffect_Curves)
		{
			Json curveJson;
			pCurve.second->SaveToJson(curveJson);
			json["SecondEffect_Curves"].push_back(curveJson);
		}
	}
	else
		json["Second_Directory"] = "";

	if (m_pThird_EffectSystem != nullptr)
	{
		json["Third_Directory"] = m_Third_EffectDirectory;

		for (auto pCurve : m_ThirdEffect_Curves)
		{
			Json curveJson;
			pCurve.second->SaveToJson(curveJson);
			json["ThirdEffect_Curves"].push_back(curveJson);
		}
	}
	else
		json["Third_Directory"] = "";

	if(m_pFourth_EffectSystem!=nullptr)
	{
		json["Fourth_Directory"] = m_Fourth_EffectDirectory;

		for (auto pCurve : m_FourthEffect_Curves)
		{
			Json curveJson;
			pCurve.second->SaveToJson(curveJson);
			json["FourthEffect_Curves"].push_back(curveJson);
		}
	}
	else
		json["Fourth_Directory"] = "";

	if(m_pFifth_EffectSystem != nullptr)
	{
		json["Fifth_Directory"] = m_Fifth_EffectDirectory;

		for (auto pCurve : m_FifthEffect_Curves)
		{
			Json curveJson;
			pCurve.second->SaveToJson(curveJson);
			json["FifthEffect_Curves"].push_back(curveJson);
		}
	}
	else
		json["Fifth_Directory"] = "";

	if(m_iSelectFinishFunc >= FUNC_PLAYFROMSTART && m_iSelectFinishFunc < FUNC_END )
		json["Finish_Function"] = m_iSelectFinishFunc;
	else
		json["Finish_Function"] = FUNC_RESET;

	if (m_fEndTime >= 0.f && 30.f >= m_fEndTime)
		json["End_Time"] = m_fEndTime;
	else
		json["End_Time"] = 3.f;
	
}

void CEffectGroup::LoadFromJson(const Json& json)
{
	CGameObject::LoadFromJson(json);

	// for (auto curveJson : json["Curves"])
	// {
		// auto pCurve = CCurveFloatImpl::Create(&curveJson);
		// m_Curves.emplace(pCurve->GetName(), pCurve);
	// }

	if (json.contains("First_Directory"))
	{
		m_First_EffectDirectory = json["First_Directory"];

		
	}

	if (json.contains("Second_Directory"))
	{
		m_Second_EffectDirectory = json["Second_Directory"];

		// if (json.contains("SecondEffect_Curves"))
		// {
		// 	for (auto curveJson : json["SecondEffect_Curves"])
		// 	{
		// 		// auto pCurve = CCurveFloatImpl::Create(&curveJson);
		// 		// m_SecondEffect_Curves.emplace(pCurve->GetName(), pCurve);
		// 		// AddEmptyCurve_ForSecond(pCurve->GetName());
		//
		// 		LoadAndSetCurve_Second(&curveJson);
		//
		// 	}
		// }
	}

	if (json.contains("Third_Directory"))
	{
		m_Third_EffectDirectory = json["Third_Directory"];

		// if (json.contains("ThirdEffect_Curves"))
		// {
		// 	for (auto curveJson : json["ThirdEffect_Curves"])
		// 	{
		// 		// auto pCurve = CCurveFloatImpl::Create(&curveJson);
		// 		// m_ThirdEffect_Curves.emplace(pCurve->GetName(), pCurve);
		// 		// AddEmptyCurve_ForThird(pCurve->GetName());
		//
		// 		LoadAndSetCurve_Third(&curveJson);
		//
		// 	}
		// }
	}

	if (json.contains("Fourth_Directory"))
	{
		m_Fourth_EffectDirectory = json["Fourth_Directory"];

		// if (json.contains("FourthEffect_Curves"))
		// {
		// 	for (auto curveJson : json["FourthEffect_Curves"])
		// 	{
		// 		// auto pCurve = CCurveFloatImpl::Create(&curveJson);
		// 		// m_FourthEffect_Curves.emplace(pCurve->GetName(), pCurve);
		// 		// AddEmptyCurve_ForFourth(pCurve->GetName());
		//
		// 		LoadAndSetCurve_Fourth(&curveJson);
		// 	}
		// }
	}

	if (json.contains("Fifth_Directory"))
	{
		m_Fifth_EffectDirectory = json["Fifth_Directory"];

		// if (json.contains("FifthEffect_Curves"))
		// {
		// 	for (auto curveJson : json["FifthEffect_Curves"])
		// 	{
		// 		// auto pCurve = CCurveFloatImpl::Create(&curveJson);
		// 		// m_FifthEffect_Curves.emplace(pCurve->GetName(), pCurve);
		// 		// AddEmptyCurve_ForFifth(pCurve->GetName());
		//
		// 		LoadAndSetCurve_Fifth(&curveJson);
		// 	}
		// }
	}

	json["End_Time"].get_to<_float>(m_fEndTime);
	json["Finish_Function"].get_to<_int>(m_iSelectFinishFunc);

	m_Timeline.SetTimelineLength((_double)m_fEndTime);

	if (m_iSelectFinishFunc == 0)
	{
		m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::PlayFromStart);
	}
	else if (m_iSelectFinishFunc == 1)
	{
		m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Reset);
	}
	else if (m_iSelectFinishFunc == 2)
	{
		m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Stop);
	}
	else if (m_iSelectFinishFunc == 3)
	{
		m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Reverse);
	}
}

inline void CEffectGroup::LoadAndSetCurve_First(Json* json)
{
	auto pCurve = CCurveFloatImpl::Create(json);
	m_FirstEffect_Curves.emplace(pCurve->GetName(), pCurve);

	if (!strcmp(pCurve->GetName(), "ObjectScale_All"))
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Scale_All, m_FirstEffect_Curves.find("ObjectScale_All")->second);
		Safe_AddRef(m_FirstEffect_Curves.find("ObjectScale_All")->second);
	}
	else if (!strcmp(pCurve->GetName(), "ObjectScale_Y"))
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Scale_Y, m_FirstEffect_Curves.find("ObjectScale_Y")->second);
		Safe_AddRef(m_FirstEffect_Curves.find("ObjectScale_Y")->second);
	}
	else if (!strcmp(pCurve->GetName(), "ObjectScale_X"))
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Scale_X, m_FirstEffect_Curves.find("ObjectScale_X")->second);
		Safe_AddRef(m_FirstEffect_Curves.find("ObjectScale_X")->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_0"))
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Floats_0, m_FirstEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_1"))
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Floats_1, m_FirstEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_2"))
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Floats_2, m_FirstEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_3"))
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Floats_3, m_FirstEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_4"))
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Floats_4, m_FirstEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_5"))
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Floats_5, m_FirstEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_6"))
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Floats_6, m_FirstEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_7"))
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Floats_7, m_FirstEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Intro_Time"))
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_IntroTime, m_FirstEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Outro_Time"))
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_OutroTime, m_FirstEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Ints_0"))
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Ints_0, m_FirstEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Color_Change"))
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_ColorChange, m_FirstEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(pCurve->GetName())->second);
	}
	else
	{
		MSG_BOX("Failed to FirstEffect SetCurve");
	}

}

void CEffectGroup::LoadAndSetCurve_Second(Json* json)
{
	auto pCurve = CCurveFloatImpl::Create(json);
	m_SecondEffect_Curves.emplace(pCurve->GetName(), pCurve);

	if (!strcmp(pCurve->GetName(), "ObjectScale_All"))
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Scale_All, m_SecondEffect_Curves.find("ObjectScale_All")->second);
		Safe_AddRef(m_SecondEffect_Curves.find("ObjectScale_All")->second);
	}
	else if (!strcmp(pCurve->GetName(), "ObjectScale_Y"))
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Scale_Y, m_SecondEffect_Curves.find("ObjectScale_Y")->second);
		Safe_AddRef(m_SecondEffect_Curves.find("ObjectScale_Y")->second);
	}
	else if (!strcmp(pCurve->GetName(), "ObjectScale_X"))
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Scale_X, m_SecondEffect_Curves.find("ObjectScale_X")->second);
		Safe_AddRef(m_SecondEffect_Curves.find("ObjectScale_X")->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_0"))
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Floats_0, m_SecondEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_1"))
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Floats_1, m_SecondEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_2"))
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Floats_2, m_SecondEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_3"))
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Floats_3, m_SecondEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_4"))
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Floats_4, m_SecondEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_5"))
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Floats_5, m_SecondEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_6"))
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Floats_6, m_SecondEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_7"))
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Floats_7, m_SecondEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Intro_Time"))
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_IntroTime, m_SecondEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Outro_Time"))
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_OutroTime, m_SecondEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Color_Change"))
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_ColorChange, m_SecondEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Ints_0"))
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Ints_0, m_SecondEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(pCurve->GetName())->second);
	}
	else
	{
		MSG_BOX("Failed to SecondEffect SetCurve");
	}
}

void CEffectGroup::LoadAndSetCurve_Third(Json* json)
{
	auto pCurve = CCurveFloatImpl::Create(json);
	m_ThirdEffect_Curves.emplace(pCurve->GetName(), pCurve);

	if (!strcmp(pCurve->GetName(), "ObjectScale_All"))
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Scale_All, m_ThirdEffect_Curves.find("ObjectScale_All")->second);
		Safe_AddRef(m_ThirdEffect_Curves.find("ObjectScale_All")->second);
	}
	else if (!strcmp(pCurve->GetName(), "ObjectScale_Y"))
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Scale_Y, m_ThirdEffect_Curves.find("ObjectScale_Y")->second);
		Safe_AddRef(m_ThirdEffect_Curves.find("ObjectScale_Y")->second);
	}
	else if (!strcmp(pCurve->GetName(), "ObjectScale_X"))
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Scale_X, m_ThirdEffect_Curves.find("ObjectScale_X")->second);
		Safe_AddRef(m_ThirdEffect_Curves.find("ObjectScale_X")->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_0"))
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Floats_0, m_ThirdEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_1"))
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Floats_1, m_ThirdEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_2"))
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Floats_2, m_ThirdEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_3"))
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Floats_3, m_ThirdEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_4"))
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Floats_4, m_ThirdEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_5"))
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Floats_5, m_ThirdEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_6"))
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Floats_6, m_ThirdEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_7"))
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Floats_7, m_ThirdEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Intro_Time"))
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_IntroTime, m_ThirdEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Outro_Time"))
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_OutroTime, m_ThirdEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Color_Change"))
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_ColorChange, m_ThirdEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Ints_0"))
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Ints_0, m_ThirdEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(pCurve->GetName())->second);
	}
	else
	{
		MSG_BOX("Failed to ThirdEffect SetCurve");
	}
}

void CEffectGroup::LoadAndSetCurve_Fourth(Json* json)
{
	auto pCurve = CCurveFloatImpl::Create(json);
	m_FourthEffect_Curves.emplace(pCurve->GetName(), pCurve);

	if (!strcmp(pCurve->GetName(), "ObjectScale_All"))
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Scale_All, m_FourthEffect_Curves.find("ObjectScale_All")->second);
		Safe_AddRef(m_FourthEffect_Curves.find("ObjectScale_All")->second);
	}
	else if (!strcmp(pCurve->GetName(), "ObjectScale_Y"))
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Scale_Y, m_FourthEffect_Curves.find("ObjectScale_Y")->second);
		Safe_AddRef(m_FourthEffect_Curves.find("ObjectScale_Y")->second);
	}
	else if (!strcmp(pCurve->GetName(), "ObjectScale_X"))
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Scale_X, m_FourthEffect_Curves.find("ObjectScale_X")->second);
		Safe_AddRef(m_FourthEffect_Curves.find("ObjectScale_X")->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_0"))
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Floats_0, m_FourthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_1"))
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Floats_1, m_FourthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_2"))
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Floats_2, m_FourthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_3"))
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Floats_3, m_FourthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_4"))
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Floats_4, m_FourthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_5"))
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Floats_5, m_FourthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_6"))
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Floats_6, m_FourthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_7"))
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Floats_7, m_FourthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Intro_Time"))
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_IntroTime, m_FourthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Outro_Time"))
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_OutroTime, m_FourthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Color_Change"))
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_ColorChange, m_FourthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Ints_0"))
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Ints_0, m_FourthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(pCurve->GetName())->second);
	}
	else
	{
		MSG_BOX("Failed to FourthEffect SetCurve");
	}
}

void CEffectGroup::LoadAndSetCurve_Fifth(Json* json)
{
	auto pCurve = CCurveFloatImpl::Create(json);
	m_FifthEffect_Curves.emplace(pCurve->GetName(), pCurve);

	if (!strcmp(pCurve->GetName(), "ObjectScale_All"))
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Scale_All, m_FifthEffect_Curves.find("ObjectScale_All")->second);
		Safe_AddRef(m_FifthEffect_Curves.find("ObjectScale_All")->second);
	}
	else if (!strcmp(pCurve->GetName(), "ObjectScale_Y"))
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Scale_Y, m_FifthEffect_Curves.find("ObjectScale_Y")->second);
		Safe_AddRef(m_FifthEffect_Curves.find("ObjectScale_Y")->second);
	}
	else if (!strcmp(pCurve->GetName(), "ObjectScale_X"))
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Scale_X, m_FifthEffect_Curves.find("ObjectScale_X")->second);
		Safe_AddRef(m_FifthEffect_Curves.find("ObjectScale_X")->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_0"))
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Floats_0, m_FifthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_1"))
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Floats_1, m_FifthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_2"))
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Floats_2, m_FifthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_3"))
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Floats_3, m_FifthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_4"))
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Floats_4, m_FifthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_5"))
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Floats_5, m_FifthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_6"))
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Floats_6, m_FifthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Floats_7"))
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Floats_7, m_FifthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Intro_Time"))
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_IntroTime, m_FifthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Outro_Time"))
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_OutroTime, m_FifthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Color_Change"))
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_ColorChange, m_FifthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(pCurve->GetName())->second);
	}
	else if (!strcmp(pCurve->GetName(), "Ints_0"))
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Ints_0, m_FifthEffect_Curves.find(pCurve->GetName())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(pCurve->GetName())->second);
	}
	else
	{
		MSG_BOX("Failed to FifthEffect SetCurve");
	}
}

void CEffectGroup::Load_EffectSystem()
{
	{
		char EffectProtoTag[MAX_PATH];
		strcpy(EffectProtoTag, m_First_EffectDirectory.c_str());
		ImGui::InputText("1_EffectDir", EffectProtoTag, MAX_PATH);
		m_First_EffectDirectory = EffectProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("Load First"))
		{
			if (m_pFirst_EffectSystem != nullptr)
				Safe_Release(m_pFirst_EffectSystem);

			if (m_First_EffectDirectory == "")
			{
				MSG_BOX("PLEASE WRITE THE DIRECTORY");
				return;
			}

			Json First_Effect = CJsonStorage::GetInstance()->FindOrLoadJson(EffectProtoTag);
			m_pFirst_EffectSystem = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectGroup", L"ProtoVFX_EffectSystem", &First_Effect));

			const string& strFileName = CGameUtils::GetFileName(m_First_EffectDirectory);
			m_mapEffectSystemTag.emplace(strFileName, m_pFirst_EffectSystem);
		}
	}
	{
		char EffectProtoTag[MAX_PATH];
		strcpy(EffectProtoTag, m_Second_EffectDirectory.c_str());
		ImGui::InputText("2_EffectDir", EffectProtoTag, MAX_PATH);
		m_Second_EffectDirectory = EffectProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("Load Second"))
		{
			if (m_pSecond_EffectSystem != nullptr)
				Safe_Release(m_pSecond_EffectSystem);

			if (m_Second_EffectDirectory == "")
			{
				MSG_BOX("PLEASE WRITE THE DIRECTORY");
				return;
			}

			Json Secon_Effect = CJsonStorage::GetInstance()->FindOrLoadJson(EffectProtoTag);
			m_pSecond_EffectSystem = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectGroup", L"ProtoVFX_EffectSystem", &Secon_Effect));

			const string& strFileName = CGameUtils::GetFileName(m_Second_EffectDirectory);
			m_mapEffectSystemTag.emplace(strFileName, m_pSecond_EffectSystem);
		}
	}
	{
		char EffectProtoTag[MAX_PATH];
		strcpy(EffectProtoTag, m_Third_EffectDirectory.c_str());
		ImGui::InputText("3_EffectDir", EffectProtoTag, MAX_PATH);
		m_Third_EffectDirectory = EffectProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("Load Third"))
		{
			if (m_pThird_EffectSystem != nullptr)
				Safe_Release(m_pThird_EffectSystem);

			if (m_Third_EffectDirectory == "")
			{
				MSG_BOX("PLEASE WRITE THE DIRECTORY");
				return;
			}

			Json Third_Effect = CJsonStorage::GetInstance()->FindOrLoadJson(EffectProtoTag);
			m_pThird_EffectSystem = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectGroup", L"ProtoVFX_EffectSystem", &Third_Effect));

			const string& strFileName = CGameUtils::GetFileName(m_Third_EffectDirectory);
			m_mapEffectSystemTag.emplace(strFileName, m_pThird_EffectSystem);
		}
	}
	{
		char EffectProtoTag[MAX_PATH];
		strcpy(EffectProtoTag, m_Fourth_EffectDirectory.c_str());
		ImGui::InputText("4_EffectDir", EffectProtoTag, MAX_PATH);
		m_Fourth_EffectDirectory = EffectProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("Load Fourth"))
		{
			if (m_pFourth_EffectSystem != nullptr)
				Safe_Release(m_pFourth_EffectSystem);

			if (m_Fourth_EffectDirectory == "")
			{
				MSG_BOX("PLEASE WRITE THE DIRECTORY");
				return;
			}

			Json Fourth_Effect = CJsonStorage::GetInstance()->FindOrLoadJson(EffectProtoTag);
			m_pFourth_EffectSystem = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectGroup", L"ProtoVFX_EffectSystem", &Fourth_Effect));

			const string& strFileName = CGameUtils::GetFileName(m_Fourth_EffectDirectory);
			m_mapEffectSystemTag.emplace(strFileName, m_pFourth_EffectSystem);
		}
	}
	{
		char EffectProtoTag[MAX_PATH];
		strcpy(EffectProtoTag, m_Fifth_EffectDirectory.c_str());
		ImGui::InputText("5_EffectDir", EffectProtoTag, MAX_PATH);
		m_Fifth_EffectDirectory = EffectProtoTag;
		ImGui::SameLine();
		if (ImGui::Button("Load Fifth"))
		{
			if (m_pFifth_EffectSystem != nullptr)
				Safe_Release(m_pFifth_EffectSystem);

			if (m_Fourth_EffectDirectory == "")
			{
				MSG_BOX("PLEASE WRITE THE DIRECTORY");
				return;
			}

			Json Fifth_Effect = CJsonStorage::GetInstance()->FindOrLoadJson(EffectProtoTag);
			m_pFifth_EffectSystem = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_Get(L"Layer_EffectGroup", L"ProtoVFX_EffectSystem", &Fifth_Effect));
			const string& strFileName = CGameUtils::GetFileName(m_Fifth_EffectDirectory);
			m_mapEffectSystemTag.emplace(strFileName, m_pFifth_EffectSystem);
		}
	}
}

void CEffectGroup::Add_TargetCurve(_uint iSelectEffect, string strCurveName)
{
	if (iSelectEffect == 0)
	{
		AddEmptyCurve_ForFirst(strCurveName);
	}
	else if (iSelectEffect == 1)
	{
		AddEmptyCurve_ForSecond(strCurveName);
	}
	else if (iSelectEffect == 2)
	{
		AddEmptyCurve_ForThird(strCurveName);
	}
	else if (iSelectEffect == 3)
	{
		AddEmptyCurve_ForFourth(strCurveName);
	}
	else if (iSelectEffect == 4)
	{
		AddEmptyCurve_ForFifth(strCurveName);
	}
	else
	{
		MSG_BOX("Failed to Add Empty Curve");
		return;
	}
}

void CEffectGroup::Imgui_RenderEffectSource(_int iSelectEffect)
{
	ImGui::BeginTabBar("EffectGroup_Sources");

	if(m_pFirst_EffectSystem != nullptr && iSelectEffect ==0)
	{
		ImGui::Begin("First_Effect");
		m_pFirst_EffectSystem->Imgui_RenderProperty();
		m_pFirst_EffectSystem->Imgui_RenderComponentProperties();
		ImGui::End();
	}
	else if (m_pSecond_EffectSystem != nullptr && iSelectEffect ==1)
	{
		ImGui::Begin("Second_Effect");
		m_pSecond_EffectSystem->Imgui_RenderProperty();
		m_pSecond_EffectSystem->GetShader()->Imgui_RenderProperty();

		ImGui::End();
	}
	else if (m_pThird_EffectSystem != nullptr&& iSelectEffect == 2)
	{
		ImGui::Begin("Third_Effect");
		m_pThird_EffectSystem->Imgui_RenderProperty();
		m_pThird_EffectSystem->GetShader()->Imgui_RenderProperty();

		ImGui::End();
	}
	else if (m_pFourth_EffectSystem != nullptr && iSelectEffect == 3)
	{
		ImGui::Begin("Fourth_Effect");
		m_pFourth_EffectSystem->Imgui_RenderProperty();
		m_pFourth_EffectSystem->GetShader()->Imgui_RenderProperty();

		ImGui::End();
	}
	else if (m_pFifth_EffectSystem != nullptr && iSelectEffect == 4)
	{
		ImGui::Begin("Fifth_Effect");
		m_pFifth_EffectSystem->Imgui_RenderProperty();
		m_pFifth_EffectSystem->GetShader()->Imgui_RenderProperty();

		ImGui::End();
	}

	ImGui::EndTabBar();

}

void CEffectGroup::SetStop()
{
	m_Timeline.PlayFromStart();
}

void CEffectGroup::SetPlay()
{
	m_Timeline.PlayFromStart();
}

void CEffectGroup::SetReverse()
{
	m_Timeline.Reverse();
}

_bool CEffectGroup::CheckPlay()
{
	return m_Timeline.IsPlay();
}

void CEffectGroup::VisibleUpdate()
{
	if (nullptr != m_pFirst_EffectSystem)
		m_pFirst_EffectSystem->SetVisible(m_bVisible);
	if (nullptr != m_pSecond_EffectSystem)
		m_pSecond_EffectSystem->SetVisible(m_bVisible);
	if (nullptr != m_pThird_EffectSystem)
		m_pThird_EffectSystem->SetVisible(m_bVisible);
	if (nullptr != m_pFourth_EffectSystem)
		m_pFourth_EffectSystem->SetVisible(m_bVisible);
	if (nullptr != m_pFifth_EffectSystem)
		m_pFifth_EffectSystem->SetVisible(m_bVisible);
}

void CEffectGroup::Set_Transform(_fmatrix matSocket)
{
	if (nullptr != m_pFirst_EffectSystem)
		m_pFirst_EffectSystem->GetTransform()->Set_WorldMatrix(matSocket);
	if (nullptr != m_pSecond_EffectSystem)
		m_pSecond_EffectSystem->GetTransform()->Set_WorldMatrix(matSocket);
	if (nullptr != m_pThird_EffectSystem)
		m_pThird_EffectSystem->GetTransform()->Set_WorldMatrix(matSocket);
	if (nullptr != m_pFourth_EffectSystem)
		m_pFourth_EffectSystem->GetTransform()->Set_WorldMatrix(matSocket);
	if (nullptr != m_pFifth_EffectSystem)
		m_pFifth_EffectSystem->GetTransform()->Set_WorldMatrix(matSocket);
}

void CEffectGroup::Start_EffectWork()
{
	m_Timeline.PlayFromStart();
}


void CEffectGroup::AddEmptyCurve(string strCurveName)
{
	auto itr = m_Curves.find(strCurveName.c_str());
	if (itr != m_Curves.end())
		return;

	auto pCurve = CCurveFloatImpl::Create(strCurveName);
	m_Curves.emplace(pCurve->GetName(), pCurve);
}

void CEffectGroup::AddEmptyCurve_ForFirst(string strCurveName)
{
	auto itr = m_FirstEffect_Curves.find(strCurveName.c_str());
	if (itr != m_FirstEffect_Curves.end())
		return;

	auto pCurve = CCurveFloatImpl::Create(strCurveName);
	m_FirstEffect_Curves.emplace(pCurve->GetName(), pCurve);

	// "ObjectScale_All", "Floats_0", "Floats_1", "Color_Change", "Emissive_Change", "Intro_Time", "Outtro_Time"

	if (strCurveName == "ObjectScale_All")
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Scale_All, m_FirstEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "ObjectScale_Y")
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Scale_Y, m_FirstEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (!strcmp(pCurve->GetName(), "ObjectScale_X"))
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Scale_X, m_FirstEffect_Curves.find("ObjectScale_X")->second);
		Safe_AddRef(m_FirstEffect_Curves.find("ObjectScale_X")->second);
	}
	else if (strCurveName == "Floats_0")
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Floats_0, m_FirstEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_1")
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Floats_1, m_FirstEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_2")
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Floats_2, m_FirstEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_3")
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Floats_3, m_FirstEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_4")
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Floats_4, m_FirstEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_5")
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Floats_5, m_FirstEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_6")
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Floats_6, m_FirstEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_7")
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Floats_7, m_FirstEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Intro_Time")
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_IntroTime, m_FirstEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Outro_Time")
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_OutroTime, m_FirstEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Color_Change")
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_ColorChange, m_FirstEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Ints_0")
	{
		m_Timeline.SetCurve(m_pFirst_EffectSystem, &CEffectSystem::Tick_Ints_0, m_FirstEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FirstEffect_Curves.find(strCurveName.c_str())->second);
	}
	else
	{
		MSG_BOX("Failed to FirstEffect SetCurve");
	}
}

void CEffectGroup::AddEmptyCurve_ForSecond(string strCurveName)
{
	auto itr = m_SecondEffect_Curves.find(strCurveName.c_str());
	if (itr != m_SecondEffect_Curves.end())
		return;

	auto pCurve = CCurveFloatImpl::Create(strCurveName);
	m_SecondEffect_Curves.emplace(pCurve->GetName(), pCurve);

	if (strCurveName == "ObjectScale_All")
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Scale_All, m_SecondEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "ObjectScale_Y")
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Scale_Y, m_SecondEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (!strcmp(pCurve->GetName(), "ObjectScale_X"))
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Scale_X, m_SecondEffect_Curves.find("ObjectScale_X")->second);
		Safe_AddRef(m_SecondEffect_Curves.find("ObjectScale_X")->second);
	}
	else if (strCurveName == "Floats_0")
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Floats_0, m_SecondEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_1")
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Floats_1, m_SecondEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_2")
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Floats_2, m_SecondEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_3")
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Floats_3, m_SecondEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_4")
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Floats_4, m_SecondEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_5")
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Floats_5, m_SecondEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_6")
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Floats_6, m_SecondEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_7")
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Floats_7, m_SecondEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Intro_Time")
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_IntroTime, m_SecondEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Outro_Time")
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_OutroTime, m_SecondEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Color_Change")
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_ColorChange, m_SecondEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Ints_0")
	{
		m_Timeline.SetCurve(m_pSecond_EffectSystem, &CEffectSystem::Tick_Ints_0, m_SecondEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_SecondEffect_Curves.find(strCurveName.c_str())->second);
	}
	else
	{
		MSG_BOX("Failed to SecondEffect SetCurve");
	}
}

void CEffectGroup::AddEmptyCurve_ForThird(string strCurveName)
{
	auto itr = m_ThirdEffect_Curves.find(strCurveName.c_str());
	if (itr != m_ThirdEffect_Curves.end())
		return;

	auto pCurve = CCurveFloatImpl::Create(strCurveName);
	m_ThirdEffect_Curves.emplace(pCurve->GetName(), pCurve);

	if (strCurveName == "ObjectScale_All")
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Scale_All, m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "ObjectScale_Y")
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Scale_Y, m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (!strcmp(pCurve->GetName(), "ObjectScale_X"))
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Scale_X, m_ThirdEffect_Curves.find("ObjectScale_X")->second);
		Safe_AddRef(m_ThirdEffect_Curves.find("ObjectScale_X")->second);
	}
	else if (strCurveName == "Floats_0")
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Floats_0, m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_1")
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Floats_1, m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_2")
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Floats_2, m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_3")
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Floats_3, m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_4")
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Floats_4, m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_5")
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Floats_5, m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_6")
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Floats_6, m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_7")
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Floats_7, m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Intro_Time")
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_IntroTime, m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Outro_Time")
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_OutroTime, m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Color_Change")
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_ColorChange, m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Ints_0")
	{
		m_Timeline.SetCurve(m_pThird_EffectSystem, &CEffectSystem::Tick_Ints_0, m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_ThirdEffect_Curves.find(strCurveName.c_str())->second);
	}
	else
	{
		MSG_BOX("Failed to ThirdEffect SetCurve");
	}
}

void CEffectGroup::AddEmptyCurve_ForFourth(string strCurveName)
{
	auto itr = m_FourthEffect_Curves.find(strCurveName.c_str());
	if (itr != m_FourthEffect_Curves.end())
		return;

	auto pCurve = CCurveFloatImpl::Create(strCurveName);
	m_FourthEffect_Curves.emplace(pCurve->GetName(), pCurve);

	if (strCurveName == "ObjectScale_All")
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Scale_All, m_FourthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "ObjectScale_Y")
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Scale_Y, m_FourthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (!strcmp(pCurve->GetName(), "ObjectScale_X"))
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Scale_X, m_FourthEffect_Curves.find("ObjectScale_X")->second);
		Safe_AddRef(m_FourthEffect_Curves.find("ObjectScale_X")->second);
	}
	else if (strCurveName == "Floats_0")
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Floats_0, m_FourthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_1")
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Floats_1, m_FourthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_2")
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Floats_2, m_FourthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_3")
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Floats_3, m_FourthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_4")
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Floats_4, m_FourthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_5")
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Floats_5, m_FourthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_6")
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Floats_6, m_FourthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_7")
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Floats_7, m_FourthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Intro_Time")
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_IntroTime, m_FourthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Outro_Time")
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_OutroTime, m_FourthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Color_Change")
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_ColorChange, m_FourthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Ints_0")
	{
		m_Timeline.SetCurve(m_pFourth_EffectSystem, &CEffectSystem::Tick_Ints_0, m_FourthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FourthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else
	{
		MSG_BOX("Failed to FourthEffect SetCurve");
	}
}

void CEffectGroup::AddEmptyCurve_ForFifth(string strCurveName)
{
	auto itr = m_FifthEffect_Curves.find(strCurveName.c_str());
	if (itr != m_FifthEffect_Curves.end())
		return;

	auto pCurve = CCurveFloatImpl::Create(strCurveName);
	m_FifthEffect_Curves.emplace(pCurve->GetName(), pCurve);

	if (strCurveName == "ObjectScale_All")
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Scale_All, m_FifthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "ObjectScale_Y")
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Scale_Y, m_FifthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (!strcmp(pCurve->GetName(), "ObjectScale_X"))
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Scale_X, m_FifthEffect_Curves.find("ObjectScale_X")->second);
		Safe_AddRef(m_FifthEffect_Curves.find("ObjectScale_X")->second);
	}
	else if (strCurveName == "Floats_0")
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Floats_0, m_FifthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_1")
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Floats_1, m_FifthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_2")
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Floats_2, m_FifthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_3")
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Floats_3, m_FifthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_4")
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Floats_4, m_FifthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_5")
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Floats_5, m_FifthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_6")
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Floats_6, m_FifthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Floats_7")
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Floats_7, m_FifthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Intro_Time")
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_IntroTime, m_FifthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Outro_Time")
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_OutroTime, m_FifthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Color_Change")
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_ColorChange, m_FifthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else if (strCurveName == "Ints_0")
	{
		m_Timeline.SetCurve(m_pFifth_EffectSystem, &CEffectSystem::Tick_Ints_0, m_FifthEffect_Curves.find(strCurveName.c_str())->second);
		Safe_AddRef(m_FifthEffect_Curves.find(strCurveName.c_str())->second);
	}
	else
	{
		MSG_BOX("Failed to FifthEffect SetCurve");
	}
}

CEffectGroup* CEffectGroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEffectGroup*		pInstance = new CEffectGroup(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffectGroup");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CEffectGroup::Clone(void* pArg)
{
	CEffectGroup*		pInstance = new CEffectGroup(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffectGroup");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffectGroup::Free()
{
	CGameObject::Free();
	Safe_Release(m_pRendererCom);

	for (auto pCurve : m_Curves)
		Safe_Release(pCurve.second);
	m_Curves.clear();

	if (!m_FirstEffect_Curves.empty())
	{
		for (auto pCurve : m_FirstEffect_Curves)
			Safe_Release(pCurve.second);
		m_FirstEffect_Curves.clear();
	}

	if (!m_SecondEffect_Curves.empty())
	{
		for (auto pCurve : m_SecondEffect_Curves)
			Safe_Release(pCurve.second);
		m_SecondEffect_Curves.clear();
	}

	if (!m_ThirdEffect_Curves.empty())
	{
		for (auto pCurve : m_ThirdEffect_Curves)
			Safe_Release(pCurve.second);
		m_ThirdEffect_Curves.clear();
	}

	if (!m_FourthEffect_Curves.empty())
	{
		for (auto pCurve : m_FourthEffect_Curves)
			Safe_Release(pCurve.second);
		m_FourthEffect_Curves.clear();
	}

	if (!m_FifthEffect_Curves.empty())
	{
		for (auto pCurve : m_FifthEffect_Curves)
			Safe_Release(pCurve.second);
		m_FifthEffect_Curves.clear();
	}

	m_mapEffectSystemTag.clear();

	if(m_pFirst_EffectSystem != nullptr)
	{
		if(m_pFirst_EffectSystem->IsDeleted() == false)
			m_pFirst_EffectSystem->SetDelete();
		Safe_Release(m_pFirst_EffectSystem);
	}
	if (m_pSecond_EffectSystem != nullptr)
	{
		if (m_pSecond_EffectSystem->IsDeleted() == false)
			m_pSecond_EffectSystem->SetDelete();
		Safe_Release(m_pSecond_EffectSystem);
	}
	if (m_pThird_EffectSystem != nullptr)
	{
		if (m_pThird_EffectSystem->IsDeleted() == false)
			m_pThird_EffectSystem->SetDelete();
		Safe_Release(m_pThird_EffectSystem);
	}
	if (m_pFourth_EffectSystem != nullptr)
	{
		if (m_pFourth_EffectSystem->IsDeleted() == false)
			m_pFourth_EffectSystem->SetDelete();
		Safe_Release(m_pFourth_EffectSystem);
	}
	if (m_pFifth_EffectSystem != nullptr)
	{
		if (m_pFifth_EffectSystem->IsDeleted() == false)
			m_pFifth_EffectSystem->SetDelete();
		Safe_Release(m_pFifth_EffectSystem);
	}

}
