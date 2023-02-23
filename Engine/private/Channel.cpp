#include "stdafx.h"
#include "..\public\Channel.h"
#include "Model.h"
#include "Bone.h"
#include "GameUtils.h"

CChannel::CChannel()
{
}

CChannel::CChannel(const CChannel& rhs)
	: m_strName(rhs.m_strName)
	, m_KeyFrames(rhs.m_KeyFrames)
{

}

/* 특정애니메이션ㄴ에서 사용되는 뼈. */
HRESULT CChannel::Initialize(HANDLE hFile)
{
	DWORD dwByte = 0;

	m_strName = CGameUtils::ReadStrFile(hFile); /* Read */

	_uint iNumKeyFrames = 0;
	ReadFile(hFile, &iNumKeyFrames, sizeof(_uint), &dwByte, nullptr); /* Read */
	m_KeyFrames.reserve(iNumKeyFrames);

	for (_uint i = 0; i < iNumKeyFrames; ++i)
	{
		KEYFRAME buffer;
		ReadFile(hFile, &buffer, sizeof(KEYFRAME), &dwByte, nullptr); /* Read */
		m_KeyFrames.push_back(buffer);
	}

	m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	return S_OK;
}

void CChannel::Link_Model(CModel* pModel)
{
	m_pBone = pModel->Get_BonePtr(m_strName);
}

void CChannel::Update_TransformMatrix(_double PlayTime)
{
	if (m_pBone == nullptr)
	{
		// IM_WARN("Channel %s is not linked", m_strName.c_str());
		return;
	}

	if (m_pBone->IsDisableAnim())
		return;

	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	_matrix			TransformMatrix;

	/* 현재 재생된 시간이 마지막 키프레임시간보다 커지며.ㄴ */
	if (PlayTime >= m_KeyFrames.back().Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else if (PlayTime <= m_KeyFrames.front().Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.front().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.front().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.front().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		_uint iFrameIdx = 0;
		while (PlayTime >= m_KeyFrames[iFrameIdx + 1].Time)
		{
			++iFrameIdx;
		}
		m_iCurFrameIdx = iFrameIdx;

		_float			fRatio = (_float)(PlayTime - m_KeyFrames[iFrameIdx].Time) /
			(m_KeyFrames[iFrameIdx + 1].Time - m_KeyFrames[iFrameIdx].Time);


		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[iFrameIdx].vScale), XMLoadFloat3(&m_KeyFrames[iFrameIdx + 1].vScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[iFrameIdx].vRotation), XMLoadFloat4(&m_KeyFrames[iFrameIdx + 1].vRotation), fRatio);
		vPosition = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[iFrameIdx].vPosition), XMLoadFloat3(&m_KeyFrames[iFrameIdx + 1].vPosition), fRatio);
		vPosition = XMVectorSetW(vPosition, 1.f);

	}

	if ("Reference" == m_strName)
	{
		m_vLocalMove = vPosition;
		vPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	}

	TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	m_pBone->Set_TransformMatrix(TransformMatrix);	
}

void CChannel::Blend_TransformMatrix(_double PlayTime, _float fBlendRatio)
{
	if (m_pBone == nullptr)
	{
		// IM_WARN("Channel %s is not linked", m_strName.c_str());
		return;
	}

	if (m_pBone->IsDisableAnim())
		return;

	_vector vBaseScale;
	_vector vBaseQut;
	_vector vBasePosition;
	XMMatrixDecompose(&vBaseScale, &vBaseQut, &vBasePosition, m_pBone->Get_TransformMatrix());

	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	_matrix			TransformMatrix;

	/* 현재 재생된 시간이 마지막 키프레임시간보다 커지며.ㄴ */
	if (PlayTime >= m_KeyFrames.back().Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else if (PlayTime <= m_KeyFrames.front().Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.front().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.front().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.front().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		_uint iFrameIdx = 0;
		while (PlayTime >= m_KeyFrames[iFrameIdx + 1].Time)
		{
			++iFrameIdx;
		}

		_float			fRatio = (_float)(PlayTime - m_KeyFrames[iFrameIdx].Time) / 
			(m_KeyFrames[iFrameIdx + 1].Time - m_KeyFrames[iFrameIdx].Time);

		vScale = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[iFrameIdx].vScale), XMLoadFloat3(&m_KeyFrames[iFrameIdx + 1].vScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[iFrameIdx].vRotation), XMLoadFloat4(&m_KeyFrames[iFrameIdx + 1].vRotation), fRatio);
		vPosition = XMVectorLerp(XMLoadFloat3(&m_KeyFrames[iFrameIdx].vPosition), XMLoadFloat3(&m_KeyFrames[iFrameIdx + 1].vPosition), fRatio);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}

	// blend
	vScale = XMVectorLerp(vBaseScale, vScale, fBlendRatio);
	vRotation = XMQuaternionSlerp(vBaseQut, vRotation, fBlendRatio);
	vPosition = XMVectorLerp(vBasePosition, vPosition, fBlendRatio);
	vPosition = XMVectorSetW(vPosition, 1.f);

	if ("Reference" == m_strName)
	{
		m_vLocalMove = vPosition;
		vPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	}

	TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	m_pBone->Set_TransformMatrix(TransformMatrix);	
}

void CChannel::Blend_Additive(_double PlayTime, _float fAdditiveRatio)
{
	if (m_pBone == nullptr)
	{
		// IM_WARN("Channel %s is not linked", m_strName.c_str());
		return;
	}

	if (m_pBone->IsDisableAnim())
		return;

	_vector vBaseScale;
	_vector vBaseQut;
	_vector vBasePosition;
	XMMatrixDecompose(&vBaseScale, &vBaseQut, &vBasePosition, m_pBone->Get_TransformMatrix());

	_vector			vRotation;

	/* 현재 재생된 시간이 마지막 키프레임시간보다 커지며.ㄴ */
	if (PlayTime >= m_KeyFrames.back().Time)
	{
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
	}
	else if (PlayTime <= m_KeyFrames.front().Time)
	{
		vRotation = XMLoadFloat4(&m_KeyFrames.front().vRotation);
	}
	else
	{
		_uint iFrameIdx = 0;
		while (PlayTime >= m_KeyFrames[iFrameIdx + 1].Time)
		{
			++iFrameIdx;
		}

		_float fRatio = (_float)(PlayTime - m_KeyFrames[iFrameIdx].Time) / 
			(m_KeyFrames[iFrameIdx + 1].Time - m_KeyFrames[iFrameIdx].Time);

		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_KeyFrames[iFrameIdx].vRotation), XMLoadFloat4(&m_KeyFrames[iFrameIdx + 1].vRotation), fRatio);
	}

	vRotation = XMQuaternionSlerp(XMQuaternionIdentity(), vRotation, fAdditiveRatio);
	vRotation = XMQuaternionMultiply(vRotation, vBaseQut);

	const _matrix TransformMatrix = XMMatrixAffineTransformation(vBaseScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vBasePosition);
	m_pBone->Set_TransformMatrix(TransformMatrix);	
}

CChannel * CChannel::Create(HANDLE hFile)
{
	CChannel*		pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(hFile)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CChannel* CChannel::Clone()
{
	return new CChannel(*this);
}

void CChannel::Free()
{
	m_KeyFrames.clear();
}
