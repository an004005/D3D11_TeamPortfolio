#include "stdafx.h"
#include "CamAnimSequencer.h"
#include "AnimCam.h"


void CCamAnimSequencer::CustomDrawCompact(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& clippingRect)
{
	if (m_pAnim == nullptr)
		return;

	m_iFrameMin = 0;
	m_iFrameMax = Sec2Frame(m_pAnim->GetDuration());

	string& strPartName = m_vecPartName[index];

	if (strPartName == "__EVENT__")
	{
		m_strHoveringEventName.clear();
		m_HoveringEventTime = 0.f;

		draw_list->PushClipRect(rc.Min, rc.Max, true);
		auto& Events = m_pAnim->GetEvents();

		for (auto itr = Events.begin(); itr != Events.end();)
		{
			float fCurFrameCnt = (float)Sec2Frame(itr->EventTime); // sec to frame
			float r = (fCurFrameCnt - m_iFrameMin) / float(m_iFrameMax - m_iFrameMin);
			float x = ImLerp(rc.Min.x, rc.Max.x, r);
			ImVec2 pta(x - 6.f, rc.Min.y + 11.5f - 6);
			ImVec2 ptb(x + 6.f, rc.Min.y + 11.5f + 6);
			ImRect rect = ImRect(pta, ptb);

			if (rect.Contains(ImGui::GetMousePos()))
			{
				draw_list->AddRectFilled(pta, ptb, 0xAA0AAA00);
				if (ImGui::IsMouseClicked(1) && MessageBox(NULL, L"Delete This Event?", L"System Message", MB_YESNO) == IDYES)
				{
					itr = Events.erase(itr);
					continue;
				}
				m_strHoveringEventName = itr->strEventName;
				m_HoveringEventTime = itr->EventTime;
				draw_list->AddText(ImVec2(x + 15, rc.Min.y + 7), 0xAA000000, itr->strEventName.c_str());


				if (ImGui::IsMouseDown(0))
				{
					float revserseR = (ImGui::GetMousePos().x - rc.Min.x) / (rc.Max.x - rc.Min.x);
					_float fReversedTime = revserseR * _float(m_iFrameMax - m_iFrameMin) + _float(m_iFrameMin);
					itr->EventTime = Frame2Sec(fReversedTime);
				}

			}
			else
				draw_list->AddRectFilled(pta, ptb, 0xAAA00000);

			++itr;
		}
		draw_list->PopClipRect();
	}
	else if (strPartName == "__Frame__")
	{
		m_HoveringKeyFrameTime = 0.f;

		auto& KeyFrames = m_pAnim->GetKeyFrames();

		draw_list->PushClipRect(rc.Min, rc.Max, true);

		for (auto itr = KeyFrames.begin(); itr != KeyFrames.end();)
		{
			float fCurFrameCnt = (_float)Sec2Frame(itr->Time); // sec to frame
			float r = (fCurFrameCnt - m_iFrameMin) / float(m_iFrameMax - m_iFrameMin);
			float x = ImLerp(rc.Min.x, rc.Max.x, r);
			ImVec2 pta(x - 6.f, rc.Min.y + 11.5f - 6);
			ImVec2 ptb(x + 6.f, rc.Min.y + 11.5f + 6);
			ImRect rect = ImRect(pta, ptb);
			draw_list->AddRectFilled(pta, ptb, 0xAAA00000);

			if (rect.Contains(ImGui::GetMousePos()))
			{
				draw_list->AddRectFilled(pta, ptb, 0xAA0AAA00);

				if (ImGui::IsMouseClicked(1) && MessageBox(NULL, L"Delete This Frame?", L"System Message", MB_YESNO) == IDYES)
				{
					itr = KeyFrames.erase(itr);
					continue;
				}

				if (ImGui::IsMouseDown(0))
				{
					float revserseR = (ImGui::GetMousePos().x - rc.Min.x) / (rc.Max.x - rc.Min.x);
					_float fReversedTime = revserseR * _float(m_iFrameMax - m_iFrameMin) + _float(m_iFrameMin);
					itr->Time = Frame2Sec(fReversedTime);
				}
				m_HoveringKeyFrameTime = itr->Time;
			}
			else
				draw_list->AddRectFilled(pta, ptb, 0xAAA00000);

			++itr;
		}
		draw_list->PopClipRect();
	}
	else if (strPartName == "__LOOKAT_")
	{
		auto& LookAt = m_pAnim->GetCamLookAt();

		draw_list->PushClipRect(rc.Min, rc.Max, true);

		for (auto itr = LookAt.begin(); itr != LookAt.end();)
		{
			float fCurFrameCnt = (_float)Sec2Frame(itr->Time);
			float r = (fCurFrameCnt - m_iFrameMin) / float(m_iFrameMax - m_iFrameMin);
			float x = ImLerp(rc.Min.x, rc.Max.x, r);
			ImVec2 pta(x - 6.f, rc.Min.y + 11.5f - 6);
			ImVec2 ptb(x + 6.f, rc.Min.y + 11.5f + 6);
			ImRect rect = ImRect(pta, ptb);

			draw_list->AddRectFilled(pta, ptb, 0xAAA00000);

			if (rect.Contains(ImGui::GetMousePos()))
			{
				draw_list->AddRectFilled(pta, ptb, 0xAA0AAA00);

				if (ImGui::IsMouseClicked(1) && MessageBox(NULL, L"Delete This Frame?", L"System Message", MB_YESNO) == IDYES)
				{
					itr = LookAt.erase(itr);
					continue;
				}

				if (ImGui::IsMouseDown(0))
				{
					float revserseR = (ImGui::GetMousePos().x - rc.Min.x) / (rc.Max.x - rc.Min.x);
					_float fReversedTime = revserseR * _float(m_iFrameMax - m_iFrameMin) + _float(m_iFrameMin);
					itr->Time = Frame2Sec(fReversedTime);
				}
				m_HoveringKeyFrameTime = itr->Time;
			}
			else
				draw_list->AddRectFilled(pta, ptb, 0xAAA00000);

			++itr;
		}
		draw_list->PopClipRect();
	}
}

void CCamAnimSequencer::SetAnim(CCamAnimation* pAnim)
{
	m_pAnim = pAnim;
	Assert(m_pAnim != nullptr);

}
