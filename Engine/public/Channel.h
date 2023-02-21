#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	CChannel(const CChannel& rhs);
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(HANDLE hFile);
	void Link_Model(class CModel* pModel);

	void Update_TransformMatrix(_double PlayTime);
	void Blend_TransformMatrix(_double PlayTime, _float fBlendRatio);
	void Blend_Additive(_double PlayTime, _float fAdditiveRatio);
	_bool IsLinked() const { return m_pBone != nullptr; }

	KEYFRAME*		GetCurKeyFrame() { return &m_KeyFrames[m_iCurFrameIdx]; }

private:
	string			m_strName;

	vector<KEYFRAME>	m_KeyFrames;

	class CBone*		m_pBone = nullptr;

private:
	_uint			m_iCurFrameIdx = 0;

public:
	static CChannel* Create(HANDLE hFile);
	CChannel* Clone();
	virtual void Free() override;
};

END