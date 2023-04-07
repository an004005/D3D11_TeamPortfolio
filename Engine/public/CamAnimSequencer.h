#pragma once
#include "ImSequencer.h"

BEGIN(Engine)

class CCamAnimSequencer : public ImSequencer::SequenceInterface
{
public:
	CCamAnimSequencer() : m_iFrameMin(0), m_iFrameMax(0)
	{
		m_vecPartName.push_back("__EVENT__");
		m_vecPartName.push_back("__Frame__");
		m_vecPartName.push_back("__LOOKAT_");
	}

public:
	virtual int GetFrameMin() const
	{
		return m_iFrameMin;
	}

	virtual int GetFrameMax() const
	{
		return m_iFrameMax;
	}

	virtual int GetItemCount() const { return (int)m_vecPartName.size(); }

	virtual int GetItemTypeCount() const { return 1; }
	virtual const char* GetItemTypeName(int typeIndex) const { return "T"; }

	virtual const char* GetItemLabel(int index) const
	{
		static char tmps[128];
		_snprintf_s(tmps, 128, "%s", m_vecPartName[index].c_str());
		return tmps;
	}

	virtual void Get(int index, int** start, int** end, int* type, unsigned int* color)
	{
		if (color)
			*color = 0xFFAA8080; // same color for everyone, return color based on type
		if (start)
			*start = &m_iFrameMin;
		if (end)
			*end = &m_iFrameMax;
		// if (type)
		// 	*type = item.mType;
	}

	virtual void Del(int index)
	{
		// string strPartName = m_vecPartName[index];
		// if (strPartName == "__EVENT__") return;
		// m_CubeAnim.mapFrame.erase(strPartName);
		// m_vecPartName.erase(m_vecPartName.begin() + index);
	}

	void SetAnim(class CCamAnimation* pAnim);


	virtual void CustomDrawCompact(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& clippingRect);

	static _float Frame2Sec(const int iFrame) { return _float(iFrame) / 1000.f; }
	static int Sec2Frame(const _float fSec) { return int(fSec * 1000.f); }


public:
	string m_strHoveringEventName;
	_float m_HoveringEventTime;

	_float m_HoveringKeyFrameTime;

	int m_iFrameMin, m_iFrameMax;
	class CCamAnimation* m_pAnim = nullptr;
	vector<string> m_vecPartName; // CubeAnimFrame의 맵 컨테이너에 임시로 index를 주기 위함
};
END