#include "stdafx.h"
#include "Animation.h"
#include "Channel.h"
#include "GameUtils.h"
#include "Model.h"
#include "JsonStorage.h"
#include "GameInstance.h"
#include "Level.h"

namespace nlohmann
{
	template <>
	struct adl_serializer<ANIM_EVENT>
	{
		static void to_json(json& j, const ANIM_EVENT& value)
		{
			j["EventTime"] = value.EventTime;
			j["EventName"] = value.strEventName;
		}

		static void from_json(const json& j, ANIM_EVENT& value)
		{
			j["EventTime"].get_to(value.EventTime);
			j["EventName"].get_to(value.strEventName);
		}
	};
}

CAnimation CAnimation::s_NullAnimation;
const string CAnimation::s_ModifyFilePath = "../Bin/Resources/Meshes/Scarlet_Nexus/AnimationModifier.json";

CAnimation::CAnimation()
	: m_bFinished(false)
	, m_bLooping(false)
{
}

CAnimation::CAnimation(const CAnimation& rhs)
	: m_strName(rhs.m_strName)
	, m_Duration(rhs.m_Duration)
	, m_TickPerSecond(rhs.m_TickPerSecond)
	, m_bFinished(rhs.m_bFinished)
	, m_bLooping(rhs.m_bLooping)
	, m_vLocalMove(rhs.m_vLocalMove)
	, m_vSpecialLocalMove(rhs.m_vSpecialLocalMove)
{
	m_Channels.reserve(rhs.m_Channels.size());
	for (auto& channel : rhs.m_Channels)
	{
		m_Channels.push_back(channel->Clone());
	}
}

HRESULT CAnimation::Initialize(const char* pAnimFilePath)
{
	_tchar szFilePath[MAX_PATH];
	CGameUtils::c2wc(pAnimFilePath, szFilePath);
	HANDLE hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		IM_ERROR("fail to open file %s", pAnimFilePath);
		return E_FAIL;
	}

	DWORD dwByte = 0;
	m_strName = CGameUtils::ReadStrFile(hFile); /* Read */
	ReadFile(hFile, &m_Duration, sizeof(_double), &dwByte, nullptr); /* Read */
	ReadFile(hFile, &m_TickPerSecond, sizeof(_double), &dwByte, nullptr); /* Read */

	_uint iNumChannels = 0;
	ReadFile(hFile, &iNumChannels, sizeof(_uint), &dwByte, nullptr); /* Read */

	m_Channels.reserve(iNumChannels);
	for (_uint i = 0; i < iNumChannels; ++i)
	{
		CChannel* pChannel = CChannel::Create(hFile);
		Assert(pChannel != nullptr);
		m_Channels.push_back(pChannel);
	}

	CloseHandle(hFile);

	m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	m_vSpecialLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	return S_OK;
}

void CAnimation::Update_Bones_SyncRatio(_double PlayTime)
{
	// 다른 애니메이션의 플레이타임과 동기화하여 애니메이션을 받아오기 위함
	for (const auto pChannel : m_Channels)
	{
		pChannel->Update_TransformMatrix(PlayTime);
	}
}

void CAnimation::Update_Bones_SyncRatio_NonLocalLock(_double PlayTime)
{
	// ���� ���������� Reference�� ���� �̵��� ����� ����
	for (const auto pChannel : m_Channels)
	{
		pChannel->Update_TransformMatrix_NonLocalLock(PlayTime);
	}
}

void CAnimation::Update_Bones(_double TimeDelta, EAnimUpdateType eType, _float fRatio)
{
	if (m_bFinished && !m_bLooping)
	{
		return;
	}

	m_bFinished = false;
	const _double PrePlayTime = m_PlayTime;
	m_PlayTime += m_TickPerSecond * TimeDelta;

	switch (eType)
	{
	case EAnimUpdateType::NORMAL:
		for (const auto pChannel : m_Channels)
		{
			pChannel->Update_TransformMatrix(m_PlayTime);

			// 로컬 이동 채널로부터	받아옴
			if ("Reference" == pChannel->GetChannelName())
			{
				m_vLocalMove = pChannel->GetLocalMove();
				m_vLocalRotation = pChannel->GetLocalRotation();
				//m_vLocalQuaternion = pChannel->GetLocalQuaternion();
				//m_vLocalEular = pChannel->GetLocalEular();
			}
			if ("Train_Root" == pChannel->GetChannelName())
			{
				m_vSpecialLocalMove = pChannel->GetSpecialLocalMove();
			}
			if ("Eff01" == pChannel->GetChannelName())
			{
				m_vEffectLocalMove = pChannel->GetEffectLocalMove();
			}
		}
		// 이벤트 실행
		for (auto& iter : m_vecEvent)
		{
			if (iter.EventTime >= PrePlayTime && iter.EventTime < m_PlayTime)
			{
				m_pModel->EventCaller(iter.strEventName);
			}
		}
		break;
	case EAnimUpdateType::BLEND:
		for (const auto pChannel : m_Channels)
		{
			pChannel->Blend_TransformMatrix(m_PlayTime, fRatio);

			if ("Reference" == pChannel->GetChannelName())
			{
				//m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
				m_vLocalMove = pChannel->GetLocalMove();
				m_vLocalRotation = pChannel->GetLocalRotation();
				//m_vLocalQuaternion = pChannel->GetLocalQuaternion();
			}
			if ("Train_Root" == pChannel->GetChannelName())
			{
				m_vSpecialLocalMove = pChannel->GetSpecialLocalMove();
			}
			if ("Eff01" == pChannel->GetChannelName())
			{
				m_vEffectLocalMove = pChannel->GetEffectLocalMove();
			}
		}
		for (auto& iter : m_vecEvent)
		{
			if (iter.EventTime >= PrePlayTime && iter.EventTime < m_PlayTime)
			{
				m_pModel->EventCaller(iter.strEventName);
			}
		}
		break;
	case EAnimUpdateType::ADDITIVE:
		for (const auto pChannel : m_Channels)
		{
			pChannel->Blend_Additive(m_PlayTime, fRatio);
		}
		break;
	case EAnimUpdateType::TYPE_END:
		FALLTHROUGH
	default: ;
		NODEFAULT
	}

	if (m_PlayTime >= m_Duration)
	{
		if (m_bStay)
		{
			m_PlayTime = m_Duration;
		}
		else
		{
			//CGameInstance*		pGameInstance = CGameInstance::GetInstance();
			if (4 == CGameInstance::GetInstance()->GetCurLevelIdx())
				m_PlayTime = 0.0;

			m_bFinished = true;
		}
	}
}

void CAnimation::Update_BonesAtTime(_double PlayTime, EAnimUpdateType eType, _float fRatio)
{
	switch (eType)
	{
	case EAnimUpdateType::NORMAL:
		for (const auto pChannel : m_Channels)
		{
			pChannel->Update_TransformMatrix(PlayTime);
		}
		break;
	case EAnimUpdateType::BLEND:
		for (const auto pChannel : m_Channels)
		{
			pChannel->Blend_TransformMatrix(PlayTime, fRatio);
		}
		break;
	case EAnimUpdateType::ADDITIVE:
		for (const auto pChannel : m_Channels)
		{
			pChannel->Blend_Additive(PlayTime, fRatio);
		}
		break;
	case EAnimUpdateType::TYPE_END:
		FALLTHROUGH
	default: ;
		NODEFAULT
	}
}

void CAnimation::Link_Model(CModel* pModel)
{
	Assert(m_pModel == nullptr); // 중복 링크 방지
	m_pModel = pModel;
	for (auto channel : m_Channels)
	{
		channel->Link_Model(pModel);
	}

	const Json& jsonAnimModifier = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Meshes/Scarlet_Nexus/AnimationModifier.json");
	if (jsonAnimModifier.contains(m_strName))
	{
		Json ModifyData = jsonAnimModifier[m_strName];
		m_Duration = ModifyData["Duration"];
		m_TickPerSecond = ModifyData["TickPerSecond"];
		m_bLooping = ModifyData["bLooping"];
		m_vecEvent = ModifyData["vecEvent"];
	}
}

void CAnimation::Reset()
{
	m_PlayTime = 0.0;
	m_bFinished = false;
}

void CAnimation::SetLocalRotation(_bool bLocalRotation)
{
	for (auto pChannel : m_Channels)
	{
		if ("Reference" == pChannel->GetChannelName())
		{
			pChannel->SetLocalRotation(bLocalRotation);
			break;
		}
	}
}

void CAnimation::SaveModifiedData(Json& json)
{
	json["Duration"] = m_Duration;
	json["TickPerSecond"] = m_TickPerSecond;
	json["bLooping"] = m_bLooping;
	json["vecEvent"] = m_vecEvent;
}

void CAnimation::Imgui_RenderProperty()
{
	_float fPlayTime = static_cast<_float>(m_PlayTime);
	_float fDuration = static_cast<_float>(m_Duration);
	_float fTickPerSecond = static_cast<_float>(m_TickPerSecond);

	ImGui::Separator();
	ImGui::Text("%s", m_strName.c_str());
	ImGui::Text("RealDuration : %.4f", fDuration / fTickPerSecond);
	ImGui::Text("PlayRatio : %.4f", static_cast<_float>(m_PlayTime / m_Duration));
	ImGui::InputFloat("TickPerSecond", &fTickPerSecond);
	ImGui::InputFloat("Duration", &fDuration);

	ImGui::SliderFloat("Cur PlayTime", &fPlayTime, 0.f, fDuration);

	ImGui::Separator();
	ImGui::Checkbox("Looping", &m_bLooping);
	if (ImGui::Button("Reset"))
		Reset();

	ImGui::Separator();
	static char szEventName[MAX_PATH]{};
	static _float fEventTime = 0.f;
	static string szItem = "";
	ImGui::InputText("EventName", szEventName, MAX_PATH);
	ImGui::InputFloat("Event PlayTime", &fEventTime);

	static char szSelectedEventName[MAX_PATH]{};
	static _float fSelectedEventTime = 0.f;
	static string szSelectedItem = "";

	// 선택된 이벤트 이름
	ImGui::Text(szSelectedItem.c_str());

	// 이벤트 뷰어
	if (ImGui::BeginListBox("Event Frame"))
	{
		for (auto& Pair : m_vecEvent)
		{
			szItem = to_string(Pair.EventTime) + " / " + Pair.strEventName;

			const bool bSelected = szItem == szSelectedItem;

			if (bSelected)
				ImGui::SetItemDefaultFocus();

			if (ImGui::Selectable(szItem.c_str()))
			{
				strcpy_s(szSelectedEventName, Pair.strEventName.c_str());
				fSelectedEventTime = Pair.EventTime;
				szSelectedItem = szItem;
			}

		}
		ImGui::EndListBox();
	}

	// 이벤트 추가
	if (ImGui::Button("Add Event"))
	{
		ANIM_EVENT AddEvent;
		ZeroMemory(&AddEvent, sizeof(ANIM_EVENT));
		AddEvent.EventTime = fEventTime;
		AddEvent.strEventName = szEventName;
		m_vecEvent.push_back(AddEvent);
	}

	// 이벤트 삭제
	if (ImGui::Button("Delete Event"))
	{
		for (auto iter = m_vecEvent.begin(); iter != m_vecEvent.end();)
		{
			if ((iter->EventTime == fSelectedEventTime) && (iter->strEventName == szSelectedEventName))
			{
				iter = m_vecEvent.erase(iter);
				szSelectedItem = "";
				break;
			}
			else
				++iter;
		}
	}

	if (ImGui::Button("Save AnimModify Data"))
	{
		Json AnimModifiers = CJsonStorage::GetInstance()->FindOrLoadJson(s_ModifyFilePath);

		Json json;
		SaveModifiedData(json);
		AnimModifiers[m_strName] = json;

		CJsonStorage::GetInstance()->UpdateJson(s_ModifyFilePath, AnimModifiers);
		CJsonStorage::GetInstance()->SaveJson(s_ModifyFilePath);
	}

	m_PlayTime = static_cast<_double>(fPlayTime);
	m_Duration = static_cast<_double>(fDuration);
	m_TickPerSecond = static_cast<_double>(fTickPerSecond);
}

KEYFRAME * CAnimation::GetCurKeyFrame()
{
	return m_Channels.front()->GetCurKeyFrame();
}

CAnimation * CAnimation::Create(const char* pAnimFilePath)
{
	CAnimation*		pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(pAnimFilePath)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	CAnimation* clone = new CAnimation(*this);

	return clone;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}

