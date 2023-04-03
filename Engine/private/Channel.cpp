#include "stdafx.h"
#include "..\public\Channel.h"
#include "Model.h"
#include "Bone.h"
#include "GameUtils.h"
#include "MathUtils.h"

CChannel::CChannel()
{
}

CChannel::CChannel(const CChannel& rhs)
	: m_strName(rhs.m_strName)
	, m_pKeyFrames(rhs.m_pKeyFrames)
	, m_iNumKeyFrame(rhs.m_iNumKeyFrame)
	, m_bClone(true)
{

}

/* Ư���ִϸ��̼Ǥ����� ���Ǵ� ��. */
HRESULT CChannel::Initialize(HANDLE hFile)
{
	DWORD dwByte = 0;

	m_strName = CGameUtils::ReadStrFile(hFile); /* Read */

	_uint iNumKeyFrames = 0;
	ReadFile(hFile, &iNumKeyFrames, sizeof(_uint), &dwByte, nullptr); /* Read */
	m_iNumKeyFrame = iNumKeyFrames;
	m_pKeyFrames = new KEYFRAME[m_iNumKeyFrame];

	for (_uint i = 0; i < iNumKeyFrames; ++i)
	{
		// KEYFRAME buffer;
		ReadFile(hFile, &m_pKeyFrames[i], sizeof(KEYFRAME), &dwByte, nullptr); /* Read */
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

	/* ���� ����� �ð��� ������ Ű�����ӽð����� Ŀ����.�� */
	if (PlayTime >= m_pKeyFrames[m_iNumKeyFrame - 1].Time)
	{
		vScale = XMLoadFloat3(&m_pKeyFrames[m_iNumKeyFrame - 1].vScale);
		vRotation = XMLoadFloat4(&m_pKeyFrames[m_iNumKeyFrame - 1].vRotation);
		vPosition = XMLoadFloat3(&m_pKeyFrames[m_iNumKeyFrame - 1].vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else if (PlayTime <= m_pKeyFrames[0].Time)
	{
		vScale = XMLoadFloat3(&m_pKeyFrames[0].vScale);
		vRotation = XMLoadFloat4(&m_pKeyFrames[0].vRotation);
		vPosition = XMLoadFloat3(&m_pKeyFrames[0].vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		_uint iFrameIdx = 0;
		while (PlayTime >= m_pKeyFrames[iFrameIdx + 1].Time)
		{
			++iFrameIdx;
		}
		m_iCurFrameIdx = iFrameIdx;

		_float			fRatio = (_float)(PlayTime - m_pKeyFrames[iFrameIdx].Time) /
			(m_pKeyFrames[iFrameIdx + 1].Time - m_pKeyFrames[iFrameIdx].Time);


		vScale = XMVectorLerp(XMLoadFloat3(&m_pKeyFrames[iFrameIdx].vScale), XMLoadFloat3(&m_pKeyFrames[iFrameIdx + 1].vScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_pKeyFrames[iFrameIdx].vRotation), XMLoadFloat4(&m_pKeyFrames[iFrameIdx + 1].vRotation), fRatio);
		vPosition = XMVectorLerp(XMLoadFloat3(&m_pKeyFrames[iFrameIdx].vPosition), XMLoadFloat3(&m_pKeyFrames[iFrameIdx + 1].vPosition), fRatio);
		vPosition = XMVectorSetW(vPosition, 1.f);

	}

	if ("Reference" == m_strName)
	{
		m_vLocalMove = vPosition;
		vPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);

		if (m_bLocalRotation)
		{
			const static Quaternion defaultQ = XMQuaternionRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));
			const static Quaternion CondugatedDefaultQ = XMQuaternionConjugate(defaultQ);

			m_vLocalRotation = XMQuaternionMultiply(CondugatedDefaultQ, vRotation);

			vRotation = defaultQ;
		}
	}

	if ("Eff01" == m_strName)
	{
		m_vEffectLocalMove = vPosition;
	}

	if ("Train_Root" == m_strName)
	{
		m_vSpecialLocalMove = vPosition;
		vPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f);
	}

	TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	m_pBone->Set_TransformMatrix(TransformMatrix);	
}

void CChannel::Update_TransformMatrix_NonLocalLock(_double PlayTime)
{
	// ������Ʈ �ִϸ��̼� �� Reference���� �̿��ϴ� �ִϸ��̼��� �־ ������ �Լ�
	// �Ϲ����� ä�� ������Ʈ���� ���X

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

	/* ���� ����� �ð��� ������ Ű�����ӽð����� Ŀ����.�� */
	if (PlayTime >= m_pKeyFrames[m_iNumKeyFrame - 1].Time)
	{
		vScale = XMLoadFloat3(&m_pKeyFrames[m_iNumKeyFrame - 1].vScale);
		vRotation = XMLoadFloat4(&m_pKeyFrames[m_iNumKeyFrame - 1].vRotation);
		vPosition = XMLoadFloat3(&m_pKeyFrames[m_iNumKeyFrame - 1].vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else if (PlayTime <= m_pKeyFrames[0].Time)
	{
		vScale = XMLoadFloat3(&m_pKeyFrames[0].vScale);
		vRotation = XMLoadFloat4(&m_pKeyFrames[0].vRotation);
		vPosition = XMLoadFloat3(&m_pKeyFrames[0].vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		_uint iFrameIdx = 0;
		while (PlayTime >= m_pKeyFrames[iFrameIdx + 1].Time)
		{
			++iFrameIdx;
		}
		m_iCurFrameIdx = iFrameIdx;

		_float			fRatio = (_float)(PlayTime - m_pKeyFrames[iFrameIdx].Time) /
			(m_pKeyFrames[iFrameIdx + 1].Time - m_pKeyFrames[iFrameIdx].Time);


		vScale = XMVectorLerp(XMLoadFloat3(&m_pKeyFrames[iFrameIdx].vScale), XMLoadFloat3(&m_pKeyFrames[iFrameIdx + 1].vScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_pKeyFrames[iFrameIdx].vRotation), XMLoadFloat4(&m_pKeyFrames[iFrameIdx + 1].vRotation), fRatio);
		vPosition = XMVectorLerp(XMLoadFloat3(&m_pKeyFrames[iFrameIdx].vPosition), XMLoadFloat3(&m_pKeyFrames[iFrameIdx + 1].vPosition), fRatio);
		vPosition = XMVectorSetW(vPosition, 1.f);

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

	_vector			vBefPosition;
	_vector			vBefRotation;

	_matrix			TransformMatrix;

	/* ���� ����� �ð��� ������ Ű�����ӽð����� Ŀ����.�� */
	if (PlayTime >= m_pKeyFrames[m_iNumKeyFrame - 1].Time)
	{
		vScale = XMLoadFloat3(&m_pKeyFrames[m_iNumKeyFrame - 1].vScale);
		vRotation = XMLoadFloat4(&m_pKeyFrames[m_iNumKeyFrame - 1].vRotation);
		vPosition = XMLoadFloat3(&m_pKeyFrames[m_iNumKeyFrame - 1].vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);

		vBefPosition = vPosition;
		vBefRotation = vRotation;
	}
	else if (PlayTime <= m_pKeyFrames[0].Time)
	{
		vScale = XMLoadFloat3(&m_pKeyFrames[0].vScale);
		vRotation = XMLoadFloat4(&m_pKeyFrames[0].vRotation);
		vPosition = XMLoadFloat3(&m_pKeyFrames[0].vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);

		vBefPosition = vPosition;
		vBefRotation = vRotation;
	}
	else
	{
		_uint iFrameIdx = 0;
		while (PlayTime >= m_pKeyFrames[iFrameIdx + 1].Time)
		{
			++iFrameIdx;
		}

		_float			fRatio = (_float)(PlayTime - m_pKeyFrames[iFrameIdx].Time) / 
			(m_pKeyFrames[iFrameIdx + 1].Time - m_pKeyFrames[iFrameIdx].Time);

		vScale = XMVectorLerp(XMLoadFloat3(&m_pKeyFrames[iFrameIdx].vScale), XMLoadFloat3(&m_pKeyFrames[iFrameIdx + 1].vScale), fRatio);
		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_pKeyFrames[iFrameIdx].vRotation), XMLoadFloat4(&m_pKeyFrames[iFrameIdx + 1].vRotation), fRatio);
		vPosition = XMVectorLerp(XMLoadFloat3(&m_pKeyFrames[iFrameIdx].vPosition), XMLoadFloat3(&m_pKeyFrames[iFrameIdx + 1].vPosition), fRatio);
		vPosition = XMVectorSetW(vPosition, 1.f);

		vBefPosition = vPosition;
		vBefRotation = vRotation;
	}

	// blend
	vScale = XMVectorLerp(vBaseScale, vScale, fBlendRatio);
	vRotation = XMQuaternionSlerp(vBaseQut, vRotation, fBlendRatio);	// Blend�ִϸ��̼��� ���� ����
	vPosition = XMVectorLerp(vBasePosition, vPosition, fBlendRatio);
	vPosition = XMVectorSetW(vPosition, 1.f);

	if ("Reference" == m_strName)
	{
		m_vLocalMove = vBefPosition;
		vRotation = vBefRotation;
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

	/* ���� ����� �ð��� ������ Ű�����ӽð����� Ŀ����.�� */
	if (PlayTime >= m_pKeyFrames[m_iNumKeyFrame - 1].Time)
	{
		vRotation = XMLoadFloat4(&m_pKeyFrames[m_iNumKeyFrame - 1].vRotation);
	}
	else if (PlayTime <= m_pKeyFrames[0].Time)
	{
		vRotation = XMLoadFloat4(&m_pKeyFrames[0].vRotation);
	}
	else
	{
		_uint iFrameIdx = 0;
		while (PlayTime >= m_pKeyFrames[iFrameIdx + 1].Time)
		{
			++iFrameIdx;
		}

		_float fRatio = (_float)(PlayTime - m_pKeyFrames[iFrameIdx].Time) / 
			(m_pKeyFrames[iFrameIdx + 1].Time - m_pKeyFrames[iFrameIdx].Time);

		vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_pKeyFrames[iFrameIdx].vRotation), XMLoadFloat4(&m_pKeyFrames[iFrameIdx + 1].vRotation), fRatio);
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
	// m_KeyFrames.clear();
	if (m_bClone == false)
		Safe_Delete_Array(m_pKeyFrames);
	m_pKeyFrames = nullptr;
}
