#include "stdafx.h"
#include "..\public\Camera_Manager.h"

#include "AnimCam.h"
#include "GameInstance.h"
#include "Object_Manager.h"
#include "PipeLine.h"
#include "Light_Manager.h"
#include "CurveManager.h"
#include "CurveFloatMapImpl.h"
#include "GameUtils.h"
#include "JsonLib.h"

IMPLEMENT_SINGLETON(CCamera_Manager);

CCamera_Manager::CCamera_Manager()
{
}

void CCamera_Manager::Tick()
{

	// if (0 == m_vecCamera.size())
	// 	return;

	// CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (m_strMainCamTag.empty() == false && GetMainCam() != nullptr)
	{
		CCamera* pMainCam = GetMainCam();
		
		CPipeLine* pPipeLine = CPipeLine::GetInstance();

		pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, pMainCam->GetXMViewMatrix());
		pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, pMainCam->GetXMProjMatrix());

		//_float4 vCamPos = pPipeLine->Get_CamPosition();
		//IM_LOG("Result : %f %f %f", vCamPos.x, vCamPos.y, vCamPos.z);
	}


	// if (pGameInstance->Get_CurLevelIndex() == 2)
	// {
	// 	CCamera* pCamera = m_vecCamera[m_iIndex];
	//
	// 	NULL_CHECK(pCamera);
	//
	// 	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	//
	// 	CTransform* pTransformCom = dynamic_cast<CTransform*>(pCamera->Get_Component(L"Com_Transform"));
	// 	CCamera::CAMERADESC tCameraDesc = pCamera->Get_CamDesc();
	//
	// 	_matrix ViewMatrix = XMMatrixInverse(nullptr, pTransformCom->Get_WorldMatrix());
	// 	pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, ViewMatrix);
	//
	// 	_matrix ProjMatrix = XMMatrixPerspectiveFovLH(tCameraDesc.fFovy, tCameraDesc.fAspect, tCameraDesc.fNear, tCameraDesc.fFar);
	// 	pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, ProjMatrix);
	//
	// 	Set_Cascade();
	// }
	//
	// if(pGameInstance->Get_CurLevelIndex() == 1)
	// {
	// 	m_vecCamera.clear();
	// }
}

void CCamera_Manager::LoadCamAnims(const string& strDir)
{
	m_strCamAnimDir = strDir;
	CGameUtils::ListFiles(strDir, [this](const string& filePath)
		{
			std::ifstream ifs(filePath);
			Json curveJson = Json::parse(ifs);
			auto pCamAnim = CCamAnimation::Create(filePath.c_str());
			m_CamAnims.emplace(pCamAnim->GetName(), pCamAnim);
		});
}

void CCamera_Manager::SaveCamAnims()
{
	for (auto camAnim : m_CamAnims)
	{
		if (auto pCamAnim = camAnim.second)
		{
			Json curveJson;
			pCamAnim->SaveToJson(curveJson);
			string curveFilePath = m_strCamAnimDir + pCamAnim->GetName() + ".json";
			std::ofstream file(curveFilePath);
			file << curveJson;
		}
	}
}

CCamAnimation* CCamera_Manager::GetCamAnim(const string& strName)
{
	const auto itr = m_CamAnims.find(strName);
	if (itr != m_CamAnims.end())
		return itr->second;
	return nullptr;
}

void CCamera_Manager::AddCamAnim(CCamAnimation* pCamAnim)
{
	m_CamAnims.emplace(pCamAnim->GetName(), pCamAnim);
}

CCamera* CCamera_Manager::Add_Camera(const string& strCamTag, _uint iLevelIdx, const wstring& pLayerTag, const wstring& pPrototypeTag, const Json* camJson)
{
	Assert(FindCamera(strCamTag) == nullptr);

	CCamera* pCamera = dynamic_cast<CCamera*>(CGameInstance::GetInstance()->Clone_GameObject_Get(iLevelIdx, pLayerTag.c_str(), pPrototypeTag.c_str(), (void*)camJson));
	NULL_CHECK(pCamera);
	Safe_AddRef(pCamera);

	if (m_strMainCamTag.empty())
		m_strMainCamTag = strCamTag;

	m_Cameras.emplace(strCamTag, pCamera);

	return pCamera;
}

void CCamera_Manager::SetMainCamera(const string& strCamTag)
{
	Assert(FindCamera(strCamTag) != nullptr);
	m_strMainCamTag = strCamTag;
}

void CCamera_Manager::SetMainCamera(CCamera* pCamera)
{
	for (auto& Cam : m_Cameras)
	{
		if (Cam.second == pCamera)
		{
			m_strMainCamTag = Cam.first;
			return;
		}
	}
}

CCamera* CCamera_Manager::GetMainCam()
{
	return FindCamera(m_strMainCamTag);
}

_matrix CCamera_Manager::GetCamViewMatrix(const string& strCamTag)
{
	auto pCam = FindCamera(strCamTag);
	Assert(pCam != nullptr);

	return pCam->GetViewMatrix();
}

_matrix CCamera_Manager::GetCamProjMatrix(const string& strCamTag)
{
	auto pCam = FindCamera(strCamTag);
	Assert(pCam != nullptr);

	return pCam->GetProjMatrix();
}

_float4 CCamera_Manager::Get_CamPosition(const string& strCamTag)
{
	auto pCam = FindCamera(strCamTag);
	Assert(pCam != nullptr);

	return pCam->GetTransform()->Get_State(CTransform::STATE_TRANSLATION);
}

void CCamera_Manager::Clear()
{
	m_strMainCamTag.clear();
	for (auto cam : m_Cameras)
		Safe_Release(cam.second);
	m_Cameras.clear();
}

void CCamera_Manager::Imgui_Render()
{
	static string strSelected;
	if (ImGui::BeginListBox("Cameras"))
	{
		for (auto& Cam : m_Cameras)
		{
			const bool bSelected = strSelected == Cam.first;
			if (bSelected)
				ImGui::SetItemDefaultFocus();
			if (ImGui::Selectable(Cam.first.c_str(), bSelected))
				strSelected = Cam.first;
		}

		ImGui::EndListBox();
	}

	if (ImGui::Button("SetMainCamera"))
	{
		if (strSelected.empty() == false)
			SetMainCamera(strSelected);
	}

	if (ImGui::CollapsingHeader("Show Selected Cam Properties"))
	{
		if (auto pCam = FindCamera(strSelected))
		{
			pCam->Imgui_RenderProperty();
			pCam->Imgui_RenderComponentProperties();
		}
	}

	static string strSelectCurve;
	if (ImGui::BeginListBox("CamAnim list"))
	{
		static char szSeachCurve[MAX_PATH] = "";
		ImGui::InputText("CamAnim Search", szSeachCurve, MAX_PATH);

		const string strSearch = szSeachCurve;
		const _bool bSearch = strSearch.empty() == false;

		for (auto& Pair : m_CamAnims)
		{
			if (bSearch)
			{
				if (Pair.first.find(strSearch) == string::npos)
					continue;
			}

			const bool bSelected = strSelectCurve == Pair.first;
			if (bSelected)
				ImGui::SetItemDefaultFocus();

			if (ImGui::Selectable(Pair.first.c_str(), bSelected))
				strSelectCurve = Pair.first;
		}

		ImGui::EndListBox();
	}
}

void CCamera_Manager::SetCameraFovCurve(const string & strCurveTag)
{
	// CGameInstance::GetInstance()->SetCameraFovCurve("");
	
	Safe_Release(m_pFovCurve);

	m_pFovCurve = CCurveManager::GetInstance()->GetCurve(strCurveTag);
	if (m_pFovCurve == nullptr)
		return;

	Safe_AddRef(m_pFovCurve);
}

void CCamera_Manager::ActionCamTickByPlayTime(_float fRatio)
{
	if (nullptr != m_pFovCurve)
	{
		CCamera* pMainCam = GetMainCam();
		Assert(pMainCam != nullptr);

//		IM_LOG("%f", m_pFovCurve->GetValue(fRatio));

		if (0.f == m_pFovCurve->GetValue(fRatio))
			int iA = 0;

		if (30.f > fRatio || 90.f < fRatio)
		{
			ReleaseCameraFovCurve();
			pMainCam->SetFOV(60.f);
			return;
		}

		pMainCam->SetFOV(m_pFovCurve->GetValue(fRatio));
	}
}

void CCamera_Manager::ReleaseCameraFovCurve()
{
	Safe_Release(m_pFovCurve);
	m_pFovCurve = nullptr;
	
	CCamera* pMainCam = GetMainCam();
	pMainCam->SetFOV(60.f);
}

void CCamera_Manager::SetCameraFov(_float fFov)
{
	CCamera* pMainCam = GetMainCam();
	Assert(pMainCam != nullptr);

	if (30.f > fFov || 90.f < fFov)
		fFov = 60.f;

	pMainCam->SetFOV(max(fFov, 1.f));
}

CCamera* CCamera_Manager::FindCamera(const string& strCamTag)
{
	auto itr = m_Cameras.find(strCamTag);
	if (itr == m_Cameras.end())
		return nullptr;

	return itr->second;
}

// void CCamera_Manager::Set_Cascade()
// {// 스태틱카메라
// 	CCamera* pCamera = m_vecCamera[m_iIndex];
// 	if (nullptr == pCamera)
// 		return;
//
// 	CTransform* pTransformCom = dynamic_cast<CTransform*>(pCamera->Get_Component(L"Com_Transform"));
// 	// 카메라의 역행렬과 시야각,화면비, 가까운 평면 Z,먼 평면 Z
// 	CCamera::CAMERADESC tCamDesc = pCamera->Get_CamDesc();
//
// 	//시야각을 이용하여 수직 시야각을 구함
// 	float fTanHalfVFov = tanf(tCamDesc.fFovy * 0.5f);
// 	// 수직 시야각을 이용하여 수평 시야각을 구함
// 	float fTanHalfHFov = fTanHalfVFov * tCamDesc.fAspect;
//
// 	//캐스케이드는 여러개 일단 한개만
// 	_float fCascadeEnd[2];
//
// 	fCascadeEnd[0] = tCamDesc.fNear;
// 	fCascadeEnd[1] = m_fFar;
// 	//fCascadeEnd[1] = tCamDesc.fFar;
//
// 	// 여러개의 절두체로 나눈다면 여기서 반복문을 실행!
//
// 	//+X,+Y 좌표에 수평,수직 시야각을 이용하여 구함. 각 부분 절두체의 가까운, 먼 평면의 값을 곱하여 4개의 점을 구함
// 	float fXN = fCascadeEnd[0] * fTanHalfHFov;
// 	float fXF = fCascadeEnd[1] * fTanHalfHFov;
// 	float fYN = fCascadeEnd[0] * fTanHalfVFov;
// 	float fYF = fCascadeEnd[1] * fTanHalfVFov;
//
// 	//+좌표값을 구하면 -좌표값을 구하여 각각의 절두체 평면을 구할수있음.
// 	//각 절두체의 Z값을 저장하여 i 가 낮은 순서로 가까운 평면, 먼평면을 구성함
// 	_float4 vFrustumCorners[8];
// 	//near Face
// 	vFrustumCorners[0] = _float4(fXN, fYN, fCascadeEnd[0], 1.f);
// 	vFrustumCorners[1] = _float4(-fXN, fYN, fCascadeEnd[0], 1.f);
// 	vFrustumCorners[2] = _float4(fXN, -fYN, fCascadeEnd[0], 1.f);
// 	vFrustumCorners[3] = _float4(-fXN, -fYN, fCascadeEnd[0], 1.f);
// 	//far Face
// 	vFrustumCorners[4] = _float4(fXF, fYF, fCascadeEnd[1], 1.f);
// 	vFrustumCorners[5] = _float4(-fXF, fYF, fCascadeEnd[1], 1.f);
// 	vFrustumCorners[6] = _float4(fXF, -fYF, fCascadeEnd[1], 1.f);
// 	vFrustumCorners[7] = _float4(-fXF, -fYF, fCascadeEnd[1], 1.f);
//
// 	CLight_Manager::GetInstance()->CascadeUpdate(pTransformCom->Get_WorldMatrix(), vFrustumCorners);
// }

void CCamera_Manager::Free()
{
	Clear();
	for (auto camAnim : m_CamAnims)
	{
		Safe_Release(camAnim.second);
	}
}
