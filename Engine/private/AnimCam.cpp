#include "stdafx.h"
#include "AnimCam.h"

#include "GameInstance.h"
#include "MathUtils.h"
#include "JsonStorage.h"
#include "CamAnimSequencer.h"

#include "PhysX_Manager.h"
#include "ImguiUtils.h"

namespace nlohmann
{
	template <>
	struct adl_serializer<CAM_ANIM_EVENT>
	{
		static void to_json(json& j, const CAM_ANIM_EVENT& value)
		{
			j["EventTime"] = value.EventTime;
			j["EventName"] = value.strEventName;
		}

		static void from_json(const json& j, CAM_ANIM_EVENT& value)
		{
			j["EventTime"].get_to(value.EventTime);
			j["EventName"].get_to(value.strEventName);
		}
	};
	template <>
	struct adl_serializer<CAM_KEYFRAME>
	{
		static void to_json(json& j, const CAM_KEYFRAME& value)
		{
			j["Time"] = value.Time;
			j["Position"] = value.vPosition;
			j["Rotation"] = value.vRotation;
			j["FOV"] = value.fFOV;
		}

		static void from_json(const json& j, CAM_KEYFRAME& value)
		{
			j["Time"].get_to(value.Time);
			j["Position"].get_to(value.vPosition);
			j["Rotation"].get_to(value.vRotation);
			value.fFOV = j["FOV"];
		}
	};
	template <>
	struct adl_serializer<CAM_LOOKAT>
	{
		static void to_json(json& j, const CAM_LOOKAT& value)
		{
			j["Time"] = value.Time;
			j["Position"] = value.vPosition;
		}

		static void from_json(const json& j, CAM_LOOKAT& value)
		{
			j["Time"].get_to(value.Time);
			j["Position"].get_to(value.vPosition);
		}
	};
}

CCamAnimation::CCamAnimation()
{
}

void CCamAnimation::Initialize(const char* pAnimFilePath)
{
	std::ifstream ifs(pAnimFilePath);
	Json json = Json::parse(ifs);

	m_strName = json["Name"];
	m_Duration = json["Duration"];
	m_TickPerSecond = json["TickPerSec"];
	m_KeyFrames = json["KeyFrames"];
	m_vecEvent = json["Events"];
	m_vecCamLookAt = json["LookAt"];
	m_fNear = json["Near"];
}

void CCamAnimation::SaveToJson(Json& json)
{
	std::sort(m_vecEvent.begin(), m_vecEvent.end(),
	[](const CAM_ANIM_EVENT& left, const CAM_ANIM_EVENT& right)
	{
		return left.EventTime < right.EventTime; // 오름차순
	});
	std::sort(m_KeyFrames.begin(), m_KeyFrames.end(),
	[](const CAM_KEYFRAME& left, const CAM_KEYFRAME& right)
	{
		return left.Time < right.Time; // 오름차순
	});
	std::sort(m_vecCamLookAt.begin(), m_vecCamLookAt.end(),
	[](const CAM_LOOKAT& left, const CAM_LOOKAT& right)
	{
		return left.Time < right.Time; // 오름차순
	});

	json["Name"] = m_strName;
	json["Duration"] = m_Duration;
	json["TickPerSec"] = m_TickPerSecond;
	json["KeyFrames"] = m_KeyFrames;
	json["Events"] = m_vecEvent;
	json["LookAt"] = m_vecCamLookAt;
	json["Near"] = m_fNear;
}

void CCamAnimation::Reset()
{
	m_PlayTime = 0.0;
	m_bFinished = false;
}

_matrix CCamAnimation::Update_Animation(_double TimeDelta, _float& fFOV, list<string>& EventNames)
{
	if (m_bFinished || m_KeyFrames.empty())
		return _float4x4::Identity;

	const _double PrePlayTime = m_PlayTime;
	m_PlayTime += m_TickPerSecond * TimeDelta;

	_vector			vRotation;
	_vector			vPosition;

	if (m_PlayTime >= m_KeyFrames.back().Time)
	{
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
		fFOV = m_KeyFrames.back().fFOV;
	}
	else if (m_PlayTime <= m_KeyFrames.front().Time)
	{
		vRotation = XMLoadFloat4(&m_KeyFrames.front().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.front().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
		fFOV = m_KeyFrames.front().fFOV;
	}
	else
	{
		_uint iFrameIdx = 0;
		while (m_PlayTime >= m_KeyFrames[iFrameIdx + 1].Time)
			++iFrameIdx;

		m_iCurFrame = iFrameIdx;

		_float fRatio = (_float)(m_PlayTime - m_KeyFrames[iFrameIdx].Time) /
			(m_KeyFrames[iFrameIdx + 1].Time - m_KeyFrames[iFrameIdx].Time);

		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[iFrameIdx].vRotation), XMLoadFloat4(&m_KeyFrames[iFrameIdx + 1].vRotation), fRatio);

		//ex)  0, 1, 2, 3, 4, 5 => 1~2 / 2~3 / 3~4 일 때 가능
		//if (iFrameIdx > 0 && iFrameIdx + 2 < m_KeyFrames.size()) // 0 초과, 마지막 인덱스 보다 2 작으면 캣멀롬 보간
		//{
		//	vPosition = XMVectorCatmullRom(
		//		XMLoadFloat3(&m_KeyFrames[iFrameIdx - 1].vPosition),
		//		XMLoadFloat3(&m_KeyFrames[iFrameIdx].vPosition), 
		//		XMLoadFloat3(&m_KeyFrames[iFrameIdx + 1].vPosition),
		//		XMLoadFloat3(&m_KeyFrames[iFrameIdx + 2].vPosition),
		//		fRatio);
		//}
		//else
		//{
			vPosition = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[iFrameIdx].vPosition), XMLoadFloat3(&m_KeyFrames[iFrameIdx + 1].vPosition), fRatio);
		//}
		vPosition = XMVectorSetW(vPosition, 1.f);
		fFOV = CMathUtils::Lerp(m_KeyFrames[iFrameIdx].fFOV, m_KeyFrames[iFrameIdx + 1].fFOV, fRatio);
	}

	for (auto& iter : m_vecEvent)
	{
		if (iter.EventTime >= PrePlayTime && iter.EventTime < m_PlayTime)
		{
			EventNames.push_back(iter.strEventName);
		}
	}

	if (m_PlayTime >= m_Duration)
	{
		m_bFinished = true;
		m_PlayTime = m_Duration;
	}
	else if (m_PlayTime < 0.0)
	{
		m_bFinished = true;
		m_PlayTime = 0.0;
	}
	

	return XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
}

_vector CCamAnimation::Update_LookAtPos(_double TimeDelta)
{
	if (m_bFinished || m_vecCamLookAt.empty())
		return _float4::Zero;

	_vector vLookAtPos;

	if (m_PlayTime >= m_vecCamLookAt.back().Time)
	{
		vLookAtPos = XMLoadFloat3(&m_vecCamLookAt.back().vPosition);
	}
	else if (m_PlayTime <= m_vecCamLookAt.front().Time)
	{
		vLookAtPos = XMLoadFloat3(&m_vecCamLookAt.front().vPosition);
	}
	else
	{
		_uint iFrameIdx = 0;
		while (m_PlayTime >= m_vecCamLookAt[iFrameIdx + 1].Time)
			++iFrameIdx;

		_float fRatio = (_float)(m_PlayTime - m_vecCamLookAt[iFrameIdx].Time) /
			(m_vecCamLookAt[iFrameIdx + 1].Time - m_vecCamLookAt[iFrameIdx].Time);


		//ex)  0, 1, 2, 3, 4, 5 => 1~2 / 2~3 / 3~4 일 때 가능
		//if (iFrameIdx > 0 && iFrameIdx + 2 < m_vecCamLookAt.size()) // 0 초과, 마지막 인덱스 보다 2 작으면 캣멀롬 보간
		//{
		//	vLookAtPos = XMVectorCatmullRom(
		//		XMLoadFloat3(&m_vecCamLookAt[iFrameIdx - 1].vPosition),
		//		XMLoadFloat3(&m_vecCamLookAt[iFrameIdx].vPosition),
		//		XMLoadFloat3(&m_vecCamLookAt[iFrameIdx + 1].vPosition),
		//		XMLoadFloat3(&m_vecCamLookAt[iFrameIdx + 2].vPosition),
		//		fRatio);
		//}
		//else
		//{
			vLookAtPos = XMVectorLerp(XMLoadFloat3(&m_vecCamLookAt[iFrameIdx].vPosition), XMLoadFloat3(&m_vecCamLookAt[iFrameIdx + 1].vPosition), fRatio);
		//}
		vLookAtPos = XMVectorSetW(vLookAtPos, 1.f);

	}

	return vLookAtPos;
}

CAM_KEYFRAME CCamAnimation::GetFirstFrame() const
{
	return m_KeyFrames.front();
}

CAM_KEYFRAME CCamAnimation::GetLastFrame() const
{
	return m_KeyFrames.back();
}

void CCamAnimation::AddFrame(_double Time, _matrix WorldMatrix, _float fFOV)
{
	_vector vScale;
	_vector vRotation;
	_vector vPosition;
	XMMatrixDecompose(&vScale, &vRotation, &vPosition, WorldMatrix);
	CAM_KEYFRAME tFrame;
	tFrame.fFOV = fFOV;
	tFrame.Time = Time;
	tFrame.vPosition = vPosition;
	tFrame.vRotation = vRotation;
	m_KeyFrames.push_back(tFrame);
	std::sort(m_KeyFrames.begin(), m_KeyFrames.end(),
	[](const CAM_KEYFRAME& left, const CAM_KEYFRAME& right)
	{
		return left.Time < right.Time; // 오름차순
	});
}

void CCamAnimation::AddFrame(_matrix WorldMatrix, _float fFOV)
{
	_vector vScale;
	_vector vRotation;
	_vector vPosition;
	XMMatrixDecompose(&vScale, &vRotation, &vPosition, WorldMatrix);
	CAM_KEYFRAME tFrame;
	tFrame.fFOV = fFOV;
	tFrame.Time = m_PlayTime;
	tFrame.vPosition = vPosition;
	tFrame.vRotation = vRotation;
	m_KeyFrames.push_back(tFrame);
	std::sort(m_KeyFrames.begin(), m_KeyFrames.end(),
		[](const CAM_KEYFRAME& left, const CAM_KEYFRAME& right)
		{
			return left.Time < right.Time; // 오름차순
		});
}

void CCamAnimation::AddEvent(_double Time, const string& strEventName)
{
	CAM_ANIM_EVENT tEvent;
	tEvent.strEventName = strEventName;
	tEvent.EventTime = Time;
	m_vecEvent.push_back(tEvent);
	std::sort(m_vecEvent.begin(), m_vecEvent.end(),
	[](const CAM_ANIM_EVENT& left, const CAM_ANIM_EVENT& right)
	{
		return left.EventTime < right.EventTime; // 오름차순
	});
}

void CCamAnimation::AddLookAt(_double Time, _fvector vPosition)
{
	CAM_LOOKAT tCamLook;
	ZeroMemory(&tCamLook, sizeof(CAM_LOOKAT));

	tCamLook.Time = m_PlayTime;
	tCamLook.vPosition = vPosition;

	m_vecCamLookAt.push_back(tCamLook);
	std::sort(m_vecCamLookAt.begin(), m_vecCamLookAt.end(),
	[](const CAM_LOOKAT& left, const CAM_LOOKAT& right)
	{
		return left.Time < right.Time;
	});
}

void CCamAnimation::Imgui_Render()
{
	_float fPlayTime = static_cast<_float>(m_PlayTime);
	_float fDuration = static_cast<_float>(m_Duration);
	_float fTickPerSecond = static_cast<_float>(m_TickPerSecond);

	ImGui::Text("Cam Anim Name : %s", m_strName.c_str());
	ImGui::Text("RealTime(Sec) : %.4f", fDuration / m_TickPerSecond);
	ImGui::Text("PlayRatio : %.4f", static_cast<_float>(m_PlayTime / m_Duration));
	ImGui::InputFloat("TickPerSecond", &fTickPerSecond);
	ImGui::InputFloat("Duration", &fDuration);
	ImGui::InputFloat("Near", &m_fNear);

	// ImGui::SliderFloat("Cur PlayTime", &fPlayTime, 0.f, fDuration);

	static CCamAnimSequencer sequencer;
	sequencer.m_pAnim = this;
	sequencer.m_iFrameMin = 0;
	sequencer.m_iFrameMax = CCamAnimSequencer::Sec2Frame(fDuration);

	int iCurrentFrame = CCamAnimSequencer::Sec2Frame(fPlayTime);
	static bool expanded = true;
	static int selectedEntry = -1;
	static int firstFrame = 0;

	string hoverEventName;

	ImGui::Text("Current Hovering Event : %s/ %f", sequencer.m_strHoveringEventName.c_str(), sequencer.m_HoveringEventTime);
	ImGui::Text("Current Hovering KeyFrameTime : %f", sequencer.m_HoveringKeyFrameTime);

	// ImGui::PopItemWidth();
	ImSequencer::Sequencer(&sequencer, &iCurrentFrame, &expanded, &selectedEntry, &firstFrame, ImSequencer::SEQUENCER_CHANGE_FRAME);

	fPlayTime = CCamAnimSequencer::Frame2Sec(iCurrentFrame);

	m_PlayTime = static_cast<_double>(fPlayTime);
	m_Duration = static_cast<_double>(fDuration);
	m_TickPerSecond = static_cast<_double>(fTickPerSecond);

	ImGui::Separator();
	static char szEventName[MAX_PATH]{};
	static string szItem = "";
	ImGui::InputText("EventName", szEventName, MAX_PATH);
	if (ImGui::Button("Add Event"))
	{
		CAM_ANIM_EVENT AddEvent;
		ZeroMemory(&AddEvent, sizeof(CAM_ANIM_EVENT));
		AddEvent.EventTime = m_PlayTime;
		AddEvent.strEventName = string(szEventName);
		m_vecEvent.push_back(AddEvent);
	}
}

CCamAnimation* CCamAnimation::Create(const char* pAnimFilePath)
{
	auto pCamAnim = new CCamAnimation;
	pCamAnim->Initialize(pAnimFilePath);
	return pCamAnim;
}

CCamAnimation* CCamAnimation::Create()
{
	auto pCamAnim =  new CCamAnimation;
	return pCamAnim;
}

void CCamAnimation::Free()
{
}


/******************
 * AnimCam
 *****************/

CAnimCam::CAnimCam(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CAnimCam::CAnimCam(const CCamera& rhs)
	: CCamera(rhs)
{
}

HRESULT CAnimCam::Initialize_Prototype()
{
	FAILED_CHECK(CCamera::Initialize_Prototype());
	

	return S_OK;
}

HRESULT CAnimCam::Initialize(void* pArg)
{
	FAILED_CHECK(CCamera::Initialize(pArg));
	CTransform::TRANSFORMDESC desc;
	desc.fSpeedPerSec = m_fSpeedPerSec;
	desc.fRotationPerSec = XMConvertToRadians(90.0f);
	m_pTransformCom->SetTransformDesc(desc);
	m_pTransformCom->Set_WorldMatrix(_float4x4::Identity);
	return S_OK;
}

void CAnimCam::Tick(_double TimeDelta)
{
	CCamera::Tick(TimeDelta);

	if (IsMainCamera() == false)
		return;

	if (m_bEditMode)
	{
		Tick_Move(TimeDelta);
		return;
	}

	if (m_bStart == false)
		return;

	if (m_bStop) TimeDelta = 0.0;

	if (m_pTargetTransform)
		m_pTransformCom->Set_WorldMatrix(m_pTargetTransform->Get_WorldMatrix());

	if (m_fStartLerpTime < m_fMaxStartLerpTime)
	{
		m_fStartLerpTime += (_float)TimeDelta;
		if (m_fStartLerpTime >= m_fMaxStartLerpTime)
			m_fStartLerpTime = m_fMaxStartLerpTime;

		const auto tFirstFrame = m_pCamAnimation->GetFirstFrame();

		_float fRatio = m_fStartLerpTime / m_fMaxStartLerpTime;

		_vector vScale;
		_vector vRotation;
		_vector vPosition;
		XMMatrixDecompose(&vScale, &vRotation, &vPosition, m_PreCamPivotMatrix);
		// 이전 캠에서 애니메이션 첫 프레임으로 보간
		vRotation = XMQuaternionSlerp(vRotation, XMLoadFloat4(&tFirstFrame.vRotation), fRatio);
		vPosition = XMVectorLerp(vPosition, XMLoadFloat3(&tFirstFrame.vPosition), fRatio);
		vPosition = XMVectorSetW(vPosition, 1.f);

		m_PivotMatrix = XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	}
	else if (m_pCamAnimation->IsFinished() && m_fEndLerpTime < m_fMaxEndLerpTime)
	{
		m_fEndLerpTime += (_float)TimeDelta;
		if (m_fEndLerpTime >= m_fMaxEndLerpTime)
		{
			m_fEndLerpTime = m_fMaxEndLerpTime;
			m_bStart = false;
			if (m_pPreCam)
			{
				m_pPreCam->SetMainCamera();
				m_pPreCam = nullptr;
				m_pTargetTransform = nullptr;
			}
		}

		const auto tLastFrame = m_pCamAnimation->GetFirstFrame();

		_float fRatio = m_fEndLerpTime / m_fMaxStartLerpTime;

		_vector vScale;
		_vector vRotation;
		_vector vPosition;
		XMMatrixDecompose(&vScale, &vRotation, &vPosition, m_PreCamPivotMatrix);
		// 애니메이션 마지막 프레임에서 이전 캠으로 보간
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&tLastFrame.vRotation), vRotation, fRatio);
		vPosition = XMVectorLerp(XMLoadFloat3(&tLastFrame.vPosition), vPosition, fRatio);
		vPosition = XMVectorSetW(vPosition, 1.f);

		m_PivotMatrix = XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	}
	else
	{
		m_RequestedEvents.clear();
		m_PivotMatrix = m_pCamAnimation->Update_Animation(TimeDelta, m_fFOV, m_RequestedEvents);

		// LookAt을 보간하여 가져온 다음 m_PivotMatrix가 해당 포인트를 바라보도록 만들기
		_vector vLerpLookAt = m_pCamAnimation->Update_LookAtPos(TimeDelta);

		if (!XMVector3Equal(vLerpLookAt, XMVectorSet(0.f, 0.f, 0.f, 0.f)))
		{
			if (XMVector3Equal(vLerpLookAt, CGameInstance::GetInstance()->Get_CamPosition()))
				return;

			_vector vDecScale, vDecRotation, vDecTrans;
			XMMatrixDecompose(&vDecScale, &vDecRotation, &vDecTrans, m_PivotMatrix);

			_matrix matRotation = XMMatrixRotationQuaternion(vDecRotation);

			_float4 vPivotRight, vPivotUp, vPivotLook, vPivotPos;
			memcpy(&vPivotRight, m_PivotMatrix.m[0], sizeof(_float4));
			memcpy(&vPivotUp, m_PivotMatrix.m[1], sizeof(_float4));
			memcpy(&vPivotLook, m_PivotMatrix.m[2], sizeof(_float4));
			memcpy(&vPivotPos, m_PivotMatrix.m[3], sizeof(_float4));

			_vector		vLook = XMVectorSetW(XMVector3Normalize(vLerpLookAt - XMLoadFloat4(&vPivotPos)), 0.f) * vPivotLook.Length();
			_vector		vRight = XMVectorSetW(XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)), 0.f) * vPivotRight.Length();
			_vector		vUp = XMVectorSetW(XMVector3Normalize(XMVector3Cross(vLook, vRight)), 0.f) * vPivotUp.Length();

			_matrix matPivot = { vRight, vUp, vLook, vPivotPos };
			matPivot = matRotation * matPivot;

			m_PivotMatrix = matPivot;
		}
		// ~LookAt

		for (auto& EventName : m_RequestedEvents)
		{
			auto itr = m_Events.find(EventName);
			if (itr != m_Events.end() && itr->second != nullptr)
				itr->second();
		}

		if (m_pCamAnimation->IsFinished() && m_fEndLerpTime >= m_fMaxEndLerpTime)
		{
			m_bStart = false;

			m_matFinishMatrix = CGameInstance::GetInstance()->Get_TransformMatrix_Inverse(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW) * m_pTransformCom->Get_WorldMatrix_Inverse();
			m_PivotMatrix = m_matFinishMatrix;

			if (m_pPreCam)
			{
				m_pPreCam->SetMainCamera();
				m_pPreCam = nullptr;
				m_pTargetTransform = nullptr;
			}
		}
	}
}

void CAnimCam::Imgui_RenderProperty()
{
	CCamera::Imgui_RenderProperty();
	ImGui::SliderFloat("MouseSpeed", &m_fMouseSpeed, 0.01f, 0.1f);
	ImGui::SliderFloat("KeyBoardSpeed", &m_fSpeedPerSec, 0.1f, 50.f);

	ImGui::Checkbox("EditMove", &m_bEditMode);
	ImGui::Checkbox("TimeDelta Stop", &m_bStop);
	ImGui::Checkbox("Play", &m_bStart);
	if (m_bEditMode)
	{
		//ImGui::Checkbox("TimeDelta Stop", &m_bStop);
		if (m_pCamAnimation)
		{
			if (m_pCamAnimation->IsFinished())
				m_pCamAnimation->SetFinish(false);

			//ImGui::Checkbox("Play", &m_bStart);

			if (ImGui::Button("Add Frame on cur anim time"))
			{
				m_pCamAnimation->AddFrame(m_PivotMatrix, m_fFOV);
			}

			if (ImGui::Button("Add LookAt By RayCast"))
			{
				// 캠에서 레이캐스트를 이용하여 바라본 지점을 첫 지정, 이후 기즈모를 이용하여 이동시킬 수 있도록 함
				physx::PxRaycastHit hitBuffer[1];
				physx::PxRaycastBuffer rayOut(hitBuffer, 1);

				RayCastParams param;
				param.rayOut = &rayOut;
				param.vOrigin = CGameInstance::GetInstance()->Get_CamPosition();
				param.vDir = CGameInstance::GetInstance()->Get_CamLook();;
				param.fDistance = 20.f;
				param.iTargetType = CTB_STATIC | CTB_PLAYER | CTB_PSYCHICK_OBJ | CTB_MONSTER | CTB_MONSTER_PART;
				param.fVisibleTime = 5.f;
				param.bSingle = true;
				if (CGameInstance::GetInstance()->RayCast(param))
				{
					for (int i = 0; i < rayOut.getNbAnyHits(); ++i)
					{
						auto pHit = rayOut.getAnyHit(i);

						_vector vLookAt = XMVectorSet(pHit.position.x, pHit.position.y, pHit.position.z, 1.f);
						
						m_pCamAnimation->AddLookAt(0.f, vLookAt);
					}
				}
				else
				{
					_vector vLookAt = XMVectorSet(0.f, 0.f, 0.f, 1.f);
					m_pCamAnimation->AddLookAt(0.f, vLookAt);
				}
			}
			// static _float fTime = 0.f;
			// ImGui::InputFloat("OnTime", &fTime);
			// if (ImGui::Button("Add Frame On Time"))
			// {
			// 	m_pCamAnimation->AddFrame(fTime, m_PivotMatrix, m_fFOV);
			// }
		}
	}

	if (ImGui::CollapsingHeader("KeyFrame_Gizmo"))
	{
		ImGui::InputInt("KeyFrame_Idx", &m_iKeyFrame_Index);
		if (m_iKeyFrame_Index < m_pCamAnimation->GetKeyFrames().size())
		{
			_float3 vPos = m_pCamAnimation->GetKeyFrames()[m_iKeyFrame_Index].vPosition;
			_float4 vRot = m_pCamAnimation->GetKeyFrames()[m_iKeyFrame_Index].vRotation;

			m_LookAtPivot = XMMatrixAffineTransformation(XMVectorSet(1.f, 1.f, 1.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 1.f), vRot, vPos);

			static GUIZMO_INFO tp1;
			CImguiUtils::Render_Guizmo(&m_LookAtPivot, tp1, true, true);

			_vector vDecScale, vDecRotation, vDecTrans;
			XMMatrixDecompose(&vDecScale, &vDecRotation, &vDecTrans, m_LookAtPivot);

			vPos = vDecTrans;
			vRot = vDecRotation;

			m_pCamAnimation->GetKeyFrames()[m_iKeyFrame_Index].vPosition = vPos;
			m_pCamAnimation->GetKeyFrames()[m_iKeyFrame_Index].vRotation = vRot;
		}
	}
	if (ImGui::CollapsingHeader("LookAt_Gizmo"))
	{
		ImGui::InputInt("LookAtPos_Idx", &m_iLookAtPos_Index);
		if (m_iLookAtPos_Index < m_pCamAnimation->GetCamLookAt().size())
		{
			_float3 vPos = m_pCamAnimation->GetCamLookAt()[m_iLookAtPos_Index].vPosition;

			m_LookAtPivot = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);

			static GUIZMO_INFO tp2;
			CImguiUtils::Render_Guizmo(&m_LookAtPivot, tp2, true, true);

			_float3 vResult = m_LookAtPivot.Translation();

			m_pCamAnimation->GetCamLookAt()[m_iLookAtPos_Index].vPosition = vResult;
		}
	}
}

void CAnimCam::Tick_Move(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_matrix WorldMatrixInv = m_pTransformCom->Get_WorldMatrix_Inverse();

	m_pTransformCom->Set_WorldMatrix(GetXMWorldMatrix());

	if (pGameInstance->KeyPressing(DIK_W))
	{
		m_pTransformCom->Go_Straight(TimeDelta);
	}

	if (pGameInstance->KeyPressing(DIK_S))
	{
		m_pTransformCom->Go_Backward(TimeDelta);
	}

	if (pGameInstance->KeyPressing(DIK_SPACE))
	{
		// m_pTransformCom->MoveVelocity(TimeDelta, _float3::UnitY * m_fSpeedPerSec);
		_float3 vVelocity = m_pTransformCom->Get_State(CTransform::STATE_UP) * m_fSpeedPerSec;
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}
	if (pGameInstance->KeyPressing(DIK_LCONTROL))
	{
		// m_pTransformCom->MoveVelocity(TimeDelta, -_float3::UnitY * m_fSpeedPerSec);
		_float3 vVelocity = -m_pTransformCom->Get_State(CTransform::STATE_UP) * m_fSpeedPerSec;
		m_pTransformCom->MoveVelocity(TimeDelta, vVelocity);
	}

	if (pGameInstance->KeyPressing(DIK_Q))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), -TimeDelta * m_fMouseSpeed);
	}
	if (pGameInstance->KeyPressing(DIK_E))
	{
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), TimeDelta * m_fMouseSpeed);
	}

	if (pGameInstance->KeyPressing(DIK_A))
	{
		m_pTransformCom->Go_Left(TimeDelta);
	}

	if (pGameInstance->KeyPressing(DIK_D))
	{
		m_pTransformCom->Go_Right(TimeDelta);
	}

	if (pGameInstance->Get_DIMouseState(CInput_Device::DIM_RB) & 0x80)
	{
		_long			MouseMove = 0;

		if (!pGameInstance->KeyDown(CInput_Device::DIM_RB))
		{
			if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_X))
			{
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * m_fMouseSpeed);
			}

			if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMS_Y))
			{
				m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove * m_fMouseSpeed);
			}
		}
	}

	m_pTransformCom->SetSpeed(m_fSpeedPerSec);

	m_PivotMatrix = m_pTransformCom->Get_WorldMatrix() * WorldMatrixInv;
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
}

void CAnimCam::StartCamAnim_Return(CCamAnimation* pCamAnim, CCamera* pPreCam, _float4x4 WorldMatrix,
	_float fStartLerpTime, _float fEndLerpTime, _bool bRotLerp)
{
	m_pPreCam = pPreCam;
	Assert(m_pPreCam != nullptr);
	StartCamAnim(pCamAnim, pPreCam->GetWorldMatrix(), WorldMatrix, fStartLerpTime, fEndLerpTime);
}

void CAnimCam::StartCamAnim(CCamAnimation* pCamAnim, _float4x4 PreCamWorldMatrix, _float4x4 WorldMatrix,
	_float fStartLerpTime, _float fEndLerpTime)
{
	Assert(pCamAnim != nullptr);
	m_pCamAnimation = pCamAnim;
	m_pCamAnimation->Reset();
	m_fNear = m_pCamAnimation->GetNear();
	m_bStart = true;
	SetMainCamera();
	m_fMaxStartLerpTime = fStartLerpTime;
	m_fStartLerpTime = 0.f;
	m_fMaxEndLerpTime = fEndLerpTime;
	m_fEndLerpTime = 0.f;

	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	const _matrix WorldMatrixInv = m_pTransformCom->Get_WorldMatrix_Inverse();
	m_PreCamPivotMatrix = PreCamWorldMatrix * WorldMatrixInv;
}

void CAnimCam::AddEvent(const string& strEventName, const function<void()>& Callback)
{
	//Assert(m_Events.find(strEventName) == m_Events.end());
	//m_Events.emplace(strEventName, Callback);

	if (m_Events.find(strEventName) == m_Events.end())
		m_Events.emplace(strEventName, Callback);
}

void CAnimCam::StartCamAnim_Return_Update(CCamAnimation* pCamAnim, CCamera* pPreCam, CTransform* pTransform, _float fStartLerpTime, _float fEndLerpTime, _bool bRotLerp)
{
	m_pPreCam = pPreCam;
	Assert(m_pPreCam != nullptr);
	StartCamAnim_Update(pCamAnim, pTransform, fStartLerpTime, fEndLerpTime);
}

void CAnimCam::StartCamAnim_Update(CCamAnimation* pCamAnim, CTransform* pTransform, _float fStartLerpTime, _float fEndLerpTime)
{
	Assert(pCamAnim != nullptr);
	m_pCamAnimation = pCamAnim;
	m_pCamAnimation->Reset();
	m_fNear = m_pCamAnimation->GetNear();
	m_bStart = true;
	SetMainCamera();
	m_fMaxStartLerpTime = fStartLerpTime;
	m_fStartLerpTime = 0.f;
	m_fMaxEndLerpTime = fEndLerpTime;
	m_fEndLerpTime = 0.f;
	Assert(pTransform != nullptr);
	m_pTargetTransform = pTransform;

	m_pTransformCom->Set_WorldMatrix(m_pTargetTransform->Get_WorldMatrix());
	const _matrix WorldMatrixInv = m_pTransformCom->Get_WorldMatrix_Inverse();
	m_PreCamPivotMatrix = pTransform->Get_WorldMatrix_f4x4() * WorldMatrixInv;
}

CAnimCam* CAnimCam::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CAnimCam* pInstance = new CAnimCam(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAnimCam");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAnimCam::Clone(void* pArg)
{
	CAnimCam* pInstance = new CAnimCam(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CAnimCam");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CAnimCam::Free()
{
	CCamera::Free();
}
