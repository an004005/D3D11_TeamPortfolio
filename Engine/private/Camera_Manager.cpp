#include "stdafx.h"
#include "..\public\Camera_Manager.h"
#include "GameInstance.h"
#include "Object_Manager.h"
#include "PipeLine.h"
#include "Light_Manager.h"
#include "CurveManager.h"
#include "CurveFloatMapImpl.h"
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

	if (m_strMainCamTag.empty() == false)
	{
		CCamera* pMainCam = GetMainCam();
		Assert(pMainCam != nullptr);

		CPipeLine* pPipeLine = CPipeLine::GetInstance();

		const _matrix ViewMatrix = pMainCam->GetViewMatrix();
		pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, ViewMatrix);
	
		const _matrix ProjMatrix = pMainCam->GetProjMatrix();
		pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, ProjMatrix);

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

CCamera* CCamera_Manager::FindCamera(const string& strCamTag)
{
	auto itr = m_Cameras.find(strCamTag);
	if (itr == m_Cameras.end())
		return nullptr;

	return itr->second;
}

// void CCamera_Manager::Set_Cascade()
// {// ����ƽī�޶�
// 	CCamera* pCamera = m_vecCamera[m_iIndex];
// 	if (nullptr == pCamera)
// 		return;
//
// 	CTransform* pTransformCom = dynamic_cast<CTransform*>(pCamera->Get_Component(L"Com_Transform"));
// 	// ī�޶��� ����İ� �þ߰�,ȭ���, ����� ��� Z,�� ��� Z
// 	CCamera::CAMERADESC tCamDesc = pCamera->Get_CamDesc();
//
// 	//�þ߰��� �̿��Ͽ� ���� �þ߰��� ����
// 	float fTanHalfVFov = tanf(tCamDesc.fFovy * 0.5f);
// 	// ���� �þ߰��� �̿��Ͽ� ���� �þ߰��� ����
// 	float fTanHalfHFov = fTanHalfVFov * tCamDesc.fAspect;
//
// 	//ĳ�����̵�� ������ �ϴ� �Ѱ���
// 	_float fCascadeEnd[2];
//
// 	fCascadeEnd[0] = tCamDesc.fNear;
// 	fCascadeEnd[1] = m_fFar;
// 	//fCascadeEnd[1] = tCamDesc.fFar;
//
// 	// �������� ����ü�� �����ٸ� ���⼭ �ݺ����� ����!
//
// 	//+X,+Y ��ǥ�� ����,���� �þ߰��� �̿��Ͽ� ����. �� �κ� ����ü�� �����, �� ����� ���� ���Ͽ� 4���� ���� ����
// 	float fXN = fCascadeEnd[0] * fTanHalfHFov;
// 	float fXF = fCascadeEnd[1] * fTanHalfHFov;
// 	float fYN = fCascadeEnd[0] * fTanHalfVFov;
// 	float fYF = fCascadeEnd[1] * fTanHalfVFov;
//
// 	//+��ǥ���� ���ϸ� -��ǥ���� ���Ͽ� ������ ����ü ����� ���Ҽ�����.
// 	//�� ����ü�� Z���� �����Ͽ� i �� ���� ������ ����� ���, ������� ������
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
}
