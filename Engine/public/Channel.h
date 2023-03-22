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
	_vector&		GetLocalMove() { return m_vLocalMove; }
	_vector&		GetSpecialLocalMove() { return m_vSpecialLocalMove; }
	_vector&		GetLocalRotation() { return m_vLocalRotation; }
	Quaternion&		GetLocalQuaternion() { return m_vLocalQuaternion; }

	pair<_vector, _float>& GetLocalEular() { return m_LocalEular; }

	const string&	GetChannelName() { return m_strName; }

	void SetLocalRotation(_bool bLocalRotation) { m_bLocalRotation = bLocalRotation; }

private:
	string			m_strName;

	vector<KEYFRAME>	m_KeyFrames;

	class CBone*		m_pBone = nullptr;

	_vector			m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector			m_vSpecialLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	_vector			m_vLocalRotation = XMQuaternionIdentity();
	Quaternion		m_vLocalQuaternion = m_vLocalQuaternion.Identity;

	pair<_vector, _float>	m_LocalEular;

private:
	_uint			m_iCurFrameIdx = 0;
	_bool			m_bLocalRotation = false;

public:
	static CChannel* Create(HANDLE hFile);
	CChannel* Clone();
	virtual void Free() override;
};

END