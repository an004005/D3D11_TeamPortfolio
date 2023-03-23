#include "stdafx.h"
#include "..\public\Model.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Bone.h"
#include "Animation.h"
#include "GameInstance.h"
#include "GameUtils.h"
#include "JsonLib.h"
#include "Material.h"
#include "ImguiUtils.h"
#include "Light_Manager.h"
#include "JsonStorage.h"
#include "PxBone.h"
#include "PxModel.h"

namespace nlohmann
{
	template <>
	struct adl_serializer<OPTIONAL_ROOTMOTION>
	{
		static void to_json(json& j, const OPTIONAL_ROOTMOTION& value)
		{
			j["AnimName"] = value.szAnimName;
			j["RootVector"] = value.vOptionalRootVector;
			j["StartTime"] = value.fStartTime;
			j["EndTime"] = value.fEndTime;
		}

		static void from_json(const json& j, OPTIONAL_ROOTMOTION& value)
		{
			j["AnimName"].get_to(value.szAnimName);
			j["RootVector"].get_to(value.vOptionalRootVector);
			j["StartTime"].get_to(value.fStartTime);
			j["EndTime"].get_to(value.fEndTime);
		}
	};
}

_float4x4 CModel::s_DefaultPivot = _float4x4::CreateScale({ 0.01f, 0.01f, 0.01f }) *_float4x4::CreateRotationY(XMConvertToRadians(-180.f));
const string CModel::s_ModifyFilePath = "../Bin/Resources/Meshes/Scarlet_Nexus/AnimationModifier.json";

CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_strName(rhs.m_strName)
	, m_eType(rhs.m_eType)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_bPx(rhs.m_bPx)
	, m_pShadowShader(rhs.m_pShadowShader)
	, m_mapOptionalRootMotion(rhs.m_mapOptionalRootMotion)
{
	if (m_eType == TYPE_ANIM)
	{
		// bone ����
		const Json jRootBone = Json::parse(rhs.m_BoneObjectDump);
		Ready_Bones(jRootBone, nullptr);
		SetBoneChildren(jRootBone);
	}

	// �޽� Ŭ��
	m_Meshes.reserve(rhs.m_Meshes.size());
	for (auto& pMesh : rhs.m_Meshes)
		m_Meshes.push_back(dynamic_cast<CMesh*>(pMesh->Clone()));

	if (m_eType == TYPE_ANIM)
	{
		// �޽� �� ����
		for (const auto pMesh : m_Meshes)
			pMesh->SetUp_MeshBones(this);

		// �ִϸ��̼� ���� �� ��ũ
		for (const auto& pAnimPair : rhs.m_mapAnimation)
		{
			m_mapAnimation.emplace(pAnimPair.first, pAnimPair.second->Clone());
		}
		for (const auto& pAnimPair : m_mapAnimation)
		{
			pAnimPair.second->Link_Model(this);
		}
	}


	// for (auto& pMtrl : m_Materials)
	// 	Safe_AddRef(pMtrl);
	//  ���͸��� ����
	m_Materials.reserve(rhs.m_Materials.size());
	for (auto pMtrl : rhs.m_Materials)
		m_Materials.push_back((CMaterial*)pMtrl->Clone(nullptr));


	Safe_AddRef(m_pShadowShader);
}

const _float3* CModel::Get_VerticesPos()
{
	return m_Meshes.front()->Get_VerticesPos();
}

const _uint CModel::Get_NumVertices()
{
	return m_Meshes.front()->Get_NumVertices();
}

const VTXMODEL* CModel::Get_NonAnimBuffer()
{
	return m_Meshes.front()->Get_NonAnimBuffer();
}




CBone * CModel::Get_BonePtr(const char * pBoneName)
{
	const auto itr = m_mapBones.find(pBoneName);
	if (itr == m_mapBones.end())
		return nullptr;

	return itr->second;
}

CBone* CModel::Get_BonePtr(const string& strBoneName)
{
	const auto itr = m_mapBones.find(strBoneName);
	if (itr == m_mapBones.end())
		return nullptr;

	return itr->second;
}

_matrix CModel::GetBoneMatrix(const string& strBoneName, _bool bPivotapply)
{
	CBone* pBone = Get_BonePtr(strBoneName);
	Assert(pBone != nullptr);

	if(bPivotapply == true)
	{
		return pBone->Get_CombindMatrix() * XMLoadFloat4x4(&m_PivotMatrix);
	}
	
	return pBone->Get_CombindMatrix();
}

CAnimation* CModel::Find_Animation(const string& strAnimaName)
{
	const auto itr = m_mapAnimation.find(strAnimaName);
	if (itr != m_mapAnimation.end())
		return itr->second;

	return nullptr;
}

_vector CModel::GetLocalMove(_fmatrix WorldMatrix)
{
	_vector vMovePos;
	ZeroMemory(&vMovePos, sizeof(_vector));

	m_vBefLocalMove = m_vLocalMove;
	m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (m_CurAnimName != "")
	{
		m_vLocalMove = m_mapAnimation[m_CurAnimName]->GetLocalMove();
		if (XMVector3Equal(m_vLocalMove, XMVectorSet(0.f, 0.f, 0.f, 0.f)))
		{
			m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_vBefLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			return XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}
		if (m_mapAnimation[m_CurAnimName]->IsFinished())
		{
			//m_fLastLocalMoveSpeed = XMVectorGetX(XMVector3Length(m_vLocalMove - m_vBefLocalMove));
			m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_vBefLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			return XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}
		if (m_LocalMoveAnimName != m_CurAnimName)
		{
			m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_vBefLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_LocalMoveAnimName = m_CurAnimName;
			return XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}

		if (m_mapAnimation[m_CurAnimName]->GetPlayRatio() < m_fBefRatio)
		{
			m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_vBefLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}

		m_fBefRatio = m_mapAnimation[m_CurAnimName]->GetPlayRatio();
	}
	m_fLastLocalMoveSpeed = XMVectorGetX(XMVector3Length(m_vLocalMove - m_vBefLocalMove));
	//m_fLastLocalMoveSpeed = 0.f;

	vMovePos = m_vLocalMove - m_vBefLocalMove;
	XMVectorSetW(vMovePos, 0.f);
	_float	fLength = XMVectorGetX(XMVector3Length(vMovePos));

	_vector vScale, vRotation, vTrans;
	XMMatrixDecompose(&vScale, &vRotation, &vTrans, WorldMatrix);
	_matrix WorldRotation = XMMatrixRotationQuaternion(vRotation);

	XMMatrixDecompose(&vScale, &vRotation, &vTrans, m_PivotMatrix);
	_matrix PivotRotation = XMMatrixRotationQuaternion(vRotation);
	vMovePos = XMVector3TransformNormal(vMovePos, PivotRotation);

	// Pivot�� ������ѵ� Y��� Z�� �̵��� ��Ʋ���� �������� ����
	// ���� X�࿡ ���� ȸ���� �ص� ����, ���� �ľ� �� �ٽ� ���󺹱� �� ��
	/*********************************************************************/
	_matrix ModifyRotation = XMMatrixRotationX(XMConvertToRadians(-90.f));
	vMovePos = XMVector3TransformNormal(vMovePos, ModifyRotation);
	/*********************************************************************/

	vMovePos = XMVector3TransformNormal(vMovePos, WorldRotation);

	XMVectorSetW(vMovePos, 0.f);

	//vMovePos *= fLength;

	return vMovePos;
}

_vector CModel::GetLocalMove(_fmatrix WorldMatrix, const string & srtAnimName)
{
	_vector vMovePos;
	ZeroMemory(&vMovePos, sizeof(_vector));

	m_vSocketBefLocalMove = m_vSocketLocalMove;
	m_vSocketLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (srtAnimName != "")
	{
		m_vSocketLocalMove = m_mapAnimation[srtAnimName]->GetLocalMove();
		if (XMVector3Equal(m_vSocketLocalMove, XMVectorSet(0.f, 0.f, 0.f, 0.f)))
		{
			m_vSocketLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_vSocketBefLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			return XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}
		if (m_mapAnimation[srtAnimName]->IsFinished())
		{
			//m_fLastLocalMoveSpeed = XMVectorGetX(XMVector3Length(m_vLocalMove - m_vBefLocalMove));
			m_vSocketLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_vSocketBefLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			return XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}
		if (m_szSocketBefAnimName != srtAnimName)
		{
			m_szSocketBefAnimName = srtAnimName;
			//m_vSocketLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_vSocketBefLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			//return XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}

		if (m_mapAnimation[srtAnimName]->GetPlayRatio() < m_fSocketBefRatio)
		{
			//m_vSocketLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_vSocketBefLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}

		m_fSocketBefRatio = m_mapAnimation[srtAnimName]->GetPlayRatio();
	}
	m_fLastLocalMoveSpeed = XMVectorGetX(XMVector3Length(m_vSocketLocalMove - m_vSocketBefLocalMove));

	vMovePos = m_vSocketLocalMove - m_vSocketBefLocalMove;
	XMVectorSetW(vMovePos, 0.f);
	_float	fLength = XMVectorGetX(XMVector3Length(vMovePos));

	_vector vScale, vRotation, vTrans;
	XMMatrixDecompose(&vScale, &vRotation, &vTrans, WorldMatrix);
	_matrix WorldRotation = XMMatrixRotationQuaternion(vRotation);

	XMMatrixDecompose(&vScale, &vRotation, &vTrans, m_PivotMatrix);
	_matrix PivotRotation = XMMatrixRotationQuaternion(vRotation);
	vMovePos = XMVector3TransformNormal(vMovePos, PivotRotation);

	// Pivot�� ������ѵ� Y��� Z�� �̵��� ��Ʋ���� �������� ����
	// ���� X�࿡ ���� ȸ���� �ص� ����, ���� �ľ� �� �ٽ� ���󺹱� �� ��
	/*********************************************************************/
	_matrix ModifyRotation = XMMatrixRotationX(XMConvertToRadians(-90.f));
	vMovePos = XMVector3TransformNormal(vMovePos, ModifyRotation);
	/*********************************************************************/

	vMovePos = XMVector3TransformNormal(vMovePos, WorldRotation);

	XMVectorSetW(vMovePos, 0.f);

	return vMovePos;
}

_float CModel::GetLocalScalar(_fmatrix WorldMatrix)
{
	_vector vMovePos;
	ZeroMemory(&vMovePos, sizeof(_vector));

	m_vBefLocalMove = m_vLocalMove;
	m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	if (m_CurAnimName != "")
	{
		m_vLocalMove = m_mapAnimation[m_CurAnimName]->GetLocalMove();
		if (XMVector3Equal(m_vLocalMove, XMVectorSet(0.f, 0.f, 0.f, 0.f)))
		{
			m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_vBefLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			return 0.f;
		}
		if (m_mapAnimation[m_CurAnimName]->IsFinished())
		{
			//m_fLastLocalMoveSpeed = XMVectorGetX(XMVector3Length(m_vLocalMove - m_vBefLocalMove));
			m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_vBefLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			return 0.f;
		}
	}
	m_fLastLocalMoveSpeed = XMVectorGetX(XMVector3Length(m_vLocalMove - m_vBefLocalMove));
	//m_fLastLocalMoveSpeed = 0.f;

	_vector vScale, vRotation, vTrans;
	XMMatrixDecompose(&vScale, &vRotation, &vTrans, WorldMatrix);
	_matrix WorldRotation = XMMatrixRotationQuaternion(vRotation);

	vMovePos = m_vLocalMove - m_vBefLocalMove;
	XMVectorSetW(vMovePos, 0.f);
	_float	fLength = XMVectorGetX(XMVector3Length(vMovePos));

	XMMatrixDecompose(&vScale, &vRotation, &vTrans, m_PivotMatrix);
	_matrix PivotRotation = XMMatrixRotationQuaternion(vRotation);
	vMovePos = XMVector3TransformNormal(vMovePos, PivotRotation);

	// Pivot�� ������ѵ� Y��� Z�� �̵��� ��Ʋ���� �������� ����
	// ���� X�࿡ ���� ȸ���� �ص� ����, ���� �ľ� �� �ٽ� ���󺹱� �� ��
	/*********************************************************************/
	_matrix ModifyRotation = XMMatrixRotationX(XMConvertToRadians(-90.f));
	vMovePos = XMVector3TransformNormal(vMovePos, ModifyRotation);
	/*********************************************************************/

	vMovePos = XMVector3TransformNormal(vMovePos, WorldRotation);

	XMVectorSetW(vMovePos, 0.f);

	//vMovePos *= fLength;

	return XMVectorGetX(XMVector3Length(vMovePos));
}

void CModel::Reset_LocalMove(_bool isSocket)
{
	if (isSocket)
	{
		m_vSocketLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		m_vSocketBefLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	}
	else
	{
		m_vLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		m_vBefLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	}
}

_matrix CModel::GetLocalRotationMatrix(_fmatrix WorldMatrix)
{
	Quaternion vDiffQuat = XMQuaternionIdentity();

	m_vBefLocalQuaternion = m_vLocalQuaternion;
	m_vLocalQuaternion = XMQuaternionIdentity();

	if (m_CurAnimName != "")
	{
		m_vLocalQuaternion = m_mapAnimation[m_CurAnimName]->GetLocalQuaternion();
		if (XMQuaternionEqual(m_vLocalQuaternion, XMQuaternionIdentity()))
		{
			m_vLocalQuaternion = XMQuaternionIdentity();
			m_vBefLocalQuaternion = XMQuaternionIdentity();

			return XMMatrixIdentity();
		}

		if (m_mapAnimation[m_CurAnimName]->IsFinished())
		{
			m_vLocalQuaternion = XMQuaternionIdentity();
			m_vBefLocalQuaternion = XMQuaternionIdentity();

			return XMMatrixIdentity();
		}
	}

	//_vector vScale, vRotation, vTrans;
	//XMMatrixDecompose(&vScale, &vRotation, &vTrans, WorldMatrix);
	//_matrix WorldRotation = XMMatrixRotationQuaternion(vRotation);

	vDiffQuat = XMQuaternionMultiply(m_vLocalQuaternion, XMQuaternionConjugate(m_vBefLocalQuaternion));

	_vector vScale, vRotation, vTrans;
	XMMatrixDecompose(&vScale, &vRotation, &vTrans, m_PivotMatrix);
	vDiffQuat = XMQuaternionMultiply(vDiffQuat, vRotation);

	static Quaternion defaultQ = XMQuaternionRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(-90.f));

	//_matrix ModifyRotation = XMMatrixRotationX(XMConvertToRadians(-90.f));
	vDiffQuat = XMQuaternionMultiply(vDiffQuat, defaultQ);

	return XMMatrixRotationQuaternion(vDiffQuat);
}

_matrix CModel::GetLocalEularMatrix(_fmatrix WorldMatrix)
{
	m_BefLocalEular = m_LocalEular;
	m_LocalEular = { XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f };

	if (m_CurAnimName != "")
	{
		m_LocalEular = m_mapAnimation[m_CurAnimName]->GetLocalEular();
		if (m_LocalEular.second == 0.f)
		{
			m_LocalEular = { XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f };
			m_BefLocalEular = { XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f };

			return XMMatrixRotationAxis(m_LocalEular.first, m_LocalEular.second);
		}

		if (m_mapAnimation[m_CurAnimName]->IsFinished())
		{
			m_LocalEular = { XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f };
			m_BefLocalEular = { XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.f };

			return XMMatrixRotationAxis(m_LocalEular.first, m_LocalEular.second);
		}
	}

	_vector vAxis = m_LocalEular.first;
	_float fDiffAngle = m_LocalEular.second - m_BefLocalEular.second;

	_vector vScale, vRotation, vTrans;
	XMMatrixDecompose(&vScale, &vRotation, &vTrans, m_PivotMatrix);
	_matrix PivotRotation = XMMatrixRotationQuaternion(vRotation);
	vAxis = XMVector3TransformNormal(vAxis, PivotRotation);

	_matrix ModifyRotation = XMMatrixRotationX(XMConvertToRadians(-90.f));
	vAxis = XMVector3TransformNormal(vAxis, ModifyRotation);

	return XMMatrixRotationAxis(vAxis, fDiffAngle);
}

_vector CModel::GetLocalRotationDelta()
{
	m_vBefLocalRotation = m_vLocalRotation;
	m_vLocalRotation = XMQuaternionIdentity();

	if (m_CurAnimName.empty() == false)
	{
		m_vLocalRotation = m_mapAnimation[m_CurAnimName]->GetLocalRotation();
		if (XMVector3Equal(m_vLocalRotation, XMQuaternionIdentity()))
		{
			m_vLocalRotation = XMQuaternionIdentity();
			m_vBefLocalRotation = XMQuaternionIdentity();
			return XMQuaternionIdentity();
		}
		if (m_mapAnimation[m_CurAnimName]->IsFinished())
		{
			m_vLocalRotation = XMQuaternionIdentity();
			m_vBefLocalRotation = XMQuaternionIdentity();
			return XMQuaternionIdentity();
		}
	}

	_vector vRotationDelta = XMQuaternionMultiply(XMQuaternionConjugate(m_vBefLocalRotation), m_vLocalRotation);
	return vRotationDelta;
}

_vector CModel::GetSpecialLocalMove(_fmatrix WorldMatrix)
{
	_vector vMovePos;
	ZeroMemory(&vMovePos, sizeof(_vector));

	m_vBefSpecialLocalMove = m_vSpecialLocalMove;
	m_vSpecialLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);

	if (m_CurAnimName != "")
	{
		m_vSpecialLocalMove = m_mapAnimation[m_CurAnimName]->GetSpecialLocalMove();

		if (XMVector3Equal(m_vSpecialLocalMove, XMVectorSet(0.f, 0.f, 0.f, 0.f)))
		{
			m_vSpecialLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_vBefSpecialLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			return XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}

		if (m_mapAnimation[m_CurAnimName]->IsFinished())
		{
			m_vSpecialLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_vBefSpecialLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			return XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}

		if (m_szBefSpecialAnimName != m_CurAnimName)
		{
//			m_vSpecialLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_vBefSpecialLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_szBefSpecialAnimName = m_CurAnimName;
//			return XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}

		if (m_mapAnimation[m_CurAnimName]->GetPlayRatio() < m_fBefSpecialRatio)
		{
//			m_vSpecialLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
			m_vBefSpecialLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
		}

		m_fBefSpecialRatio = m_mapAnimation[m_CurAnimName]->GetPlayRatio();
	}

	vMovePos = m_vSpecialLocalMove - m_vBefSpecialLocalMove;
	XMVectorSetW(vMovePos, 0.f);

	_vector vScale, vRotation, vTrans;
	XMMatrixDecompose(&vScale, &vRotation, &vTrans, WorldMatrix);
	_matrix WorldRotation = XMMatrixRotationQuaternion(vRotation);

	XMMatrixDecompose(&vScale, &vRotation, &vTrans, m_PivotMatrix);
	_matrix PivotRotation = XMMatrixRotationQuaternion(vRotation);
	vMovePos = XMVector3TransformNormal(vMovePos, PivotRotation);

	//_matrix ModifyRotation = XMMatrixRotationX(XMConvertToRadians(-90.f));
	//vMovePos = XMVector3TransformNormal(vMovePos, ModifyRotation);

	vMovePos = XMVector3TransformNormal(vMovePos, WorldRotation);

	XMVectorSetW(vMovePos, 0.f);

	return vMovePos;
}

void CModel::Reset_SpecialLocalMove()
{
	m_vSpecialLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
	m_vBefSpecialLocalMove = XMVectorSet(0.f, 0.f, 0.f, 0.f);
}

HRESULT CModel::Initialize_Prototype(const char * pModelFilePath)
{
	char szExt[MAX_PATH];
	_splitpath_s(pModelFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);
	if (strcmp(szExt, ".anim_model") == 0)
	{
		m_eType = TYPE_ANIM;
	}
	else if (strcmp(szExt, ".static_model") == 0)
	{
		m_eType = TYPE_NONANIM;
	}
	else
	{
		IM_ERROR("Model Type is not defined : %s", pModelFilePath);
		NODEFAULT
	}

	_tchar szFilePath[MAX_PATH];
	CGameUtils::c2wc(pModelFilePath, szFilePath);
	HANDLE hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		IM_ERROR("fail to open file %s", pModelFilePath);
		return E_FAIL;
	}

	DWORD dwByte = 0;

	m_strName = CGameUtils::ReadStrFile(hFile); /* Read */

	_uint iNumMeshes = 0;
	ReadFile(hFile, &iNumMeshes, sizeof(_uint), &dwByte, nullptr); /* Read */

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		
		CMesh*		pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, hFile, this);
		Assert(pMesh != nullptr);
		m_Meshes.push_back(pMesh);

	}

	Ready_Materials(hFile);
	
	if (m_eType == TYPE_ANIM)
	{
		m_BoneObjectDump = CGameUtils::ReadStrFile(hFile); /* Read */
	}

	if (m_eType == TYPE_ANIM)
	{
		m_pShadowShader = dynamic_cast<CShader*>(CGameInstance::GetInstance()
			->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxAnimModel_Shadow"));
	}
	else if (m_eType == TYPE_NONANIM)
	{
		m_pShadowShader = dynamic_cast<CShader*>(CGameInstance::GetInstance()
			->Clone_Component(LEVEL_STATIC, L"Prototype_Component_Shader_VtxModel_Shadow"));
	}

	if (m_eType == TYPE_ANIM)
	{
		const Json& jsonAnimModifier = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Meshes/Scarlet_Nexus/AnimationModifier.json");
		if (jsonAnimModifier.contains(m_strName))
		{
			Json ModifyData = jsonAnimModifier[m_strName];
			m_mapOptionalRootMotion = ModifyData["mapRootMotion"];
		}
	}

	CloseHandle(hFile);
	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

void CModel::Imgui_RenderProperty()
{
	ImGui::Text("%s", m_strName.c_str());

	if (m_eType == TYPE_ANIM)
	{
		if (ImGui::CollapsingHeader("Pivot Editor"))
		{
			ImGui::Text("Pivot");
			static GUIZMO_INFO tInfo;
			CImguiUtils::Render_Guizmo(&m_PivotMatrix, tInfo, true, true);
		}

		ImGui::Separator();
		if (ImGui::CollapsingHeader("Anim Viewer"))
		{
			static char szSeachAnim[MAX_PATH] = "";
			ImGui::InputText("Anim Search", szSeachAnim, MAX_PATH);

			const string strSearch = szSeachAnim;
			const _bool bSearch = strSearch.empty() == false;

			if (ImGui::BeginListBox("Anim list"))
			{
				for (auto& Pair : m_mapAnimation)
				{
					if (bSearch)
					{
						if (Pair.first.find(strSearch) == string::npos)
							continue;
					}

					const bool bSelected = m_CurAnimName == Pair.first;
					if (bSelected)
						ImGui::SetItemDefaultFocus();

					if (ImGui::Selectable(Pair.first.c_str(), bSelected))
						m_CurAnimName = Pair.first;
				}
				ImGui::EndListBox();
			}
		}
	}

	if (m_eType == TYPE_NONANIM)
	{
		ImGui::Text("Pivot");
		static GUIZMO_INFO tInfo;
		CImguiUtils::Render_Guizmo(&m_PivotMatrix, tInfo, true, true);

		ImGui::Separator();
		if (ImGui::CollapsingHeader("Anim Viewer"))
		{
			static char szSeachAnim[MAX_PATH] = "";
			ImGui::InputText("Anim Search", szSeachAnim, MAX_PATH);

			const string strSearch = szSeachAnim;
			const _bool bSearch = strSearch.empty() == false;

			if (ImGui::BeginListBox("Anim list"))
			{
				for (auto& Pair : m_mapAnimation)
				{
					if (bSearch)
					{
						if (Pair.first.find(strSearch) == string::npos)
							continue;
					}

					const bool bSelected = m_CurAnimName == Pair.first;
					if (bSelected)
						ImGui::SetItemDefaultFocus();

					if (ImGui::Selectable(Pair.first.c_str(), bSelected))
						m_CurAnimName = Pair.first;
				}
				ImGui::EndListBox();
			}
		}
	}


	if (ImGui::CollapsingHeader("Additive anim view"))
	{
		 if (ImGui::BeginListBox("Animations Additive"))
		 {
			 for (auto& iter : m_mapAnimation)
			 {
				 const bool bSelected = m_szAdditiveAnimName == iter.first;
				 if (bSelected)
					 ImGui::SetItemDefaultFocus();

				 if (ImGui::Selectable(iter.second->GetName().c_str(), bSelected))
				 {
					 m_szAdditiveAnimName = iter.first;
				 }
			 }
		
	 		ImGui::EndListBox();
		 }
		 ImGui::SliderFloat("additiveRatio", &m_fAdditiveRatio, 0.f, 3.f);

		 if (ImGui::Button("Additive Delete"))
		 {
			 m_mapAnimation[m_szAdditiveAnimName]->Reset();
			 m_szAdditiveAnimName = "";
		 }
	}



	if (ImGui::CollapsingHeader("Material Viewer"))
	{
		static char szSeachMtrl[MAX_PATH] = "";
		ImGui::InputText("Mtrl Seach", szSeachMtrl, MAX_PATH);

		const string strSearch = szSeachMtrl;
		const _bool bSearch = strSearch.empty() == false;

		static int iSelected = 0;

		if (!m_Materials.empty() && ImGui::BeginListBox("Material List"))
		{
			for (int i = 0; i < int(m_Materials.size()); ++i)
			{
				char protoTag[MAX_PATH];
				CGameUtils::wc2c(m_Materials[i]->GetPrototypeTag(), protoTag);
				if (bSearch)
				{
					string strProtoTag = protoTag;
					if (strProtoTag.find(strSearch) == string::npos)
						continue;
				}

				const bool bSelected = iSelected == i;
				if (bSelected)
					ImGui::SetItemDefaultFocus();

				if (ImGui::Selectable(protoTag, bSelected))
					iSelected = i;
			}
			ImGui::EndListBox();

			m_Materials[iSelected]->Imgui_RenderProperty();
		}
	}

	if (m_eType == TYPE_ANIM)
	{
		static string strSelected = m_pRootBone->Get_Name();
		static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
		if (ImGui::CollapsingHeader("Skeletal Viewer"))
		{
			m_pRootBone->Imgui_SkeletalViewRecursive(strSelected, baseFlags);

			if (strSelected.empty() == false)
			{
				CBone* pBone = Get_BonePtr(strSelected);
				_matrix tmp =  pBone->Get_CombindMatrix() * XMLoadFloat4x4(&m_PivotMatrix);
				_float4x4 tmp2;
				XMStoreFloat4x4(&tmp2, tmp);

				static GUIZMO_INFO tInfo;
				CImguiUtils::Render_Guizmo(&tmp2, tInfo, false, true);
			}
		}
	}

	if (m_eType == TYPE_ANIM)
	{
		if (ImGui::CollapsingHeader("Event Viewer"))
		{
			if (ImGui::BeginListBox("Event View"))
			{
				for (auto& iter : m_EventFunc)
				{
					if (ImGui::Selectable(iter.first.c_str()))
						iter.second();
				}
				ImGui::EndListBox();
			}
		}

		static string szSelectedEvent;

		if (ImGui::CollapsingHeader("OptionalVector Viewer"))
		{
			if (ImGui::BeginListBox("Optional Anim View"))
			{
				for (auto& iter : m_mapOptionalRootMotion)
				{
					if (iter.first == szSelectedEvent)
					{
						ImGui::SetItemDefaultFocus();
					}

					if (ImGui::Selectable(iter.first.c_str()))
					{
						szSelectedEvent = iter.first;
					}
				}
				ImGui::EndListBox();
			}

			if (ImGui::BeginListBox("OptionalVector View"))
			{
				for (auto& iter : m_mapOptionalRootMotion)
				{
					if (iter.first == szSelectedEvent)
					{
						for (auto Vector = iter.second.begin(); Vector != iter.second.end();)
						{
							if (ImGui::Selectable(to_string(Vector->fStartTime).c_str()))
							{
								Vector = iter.second.erase(Vector);
								break;
							}
							else
								++Vector;
						}
					}
				}
				ImGui::EndListBox();
			}

			if (ImGui::Button("Save OptionalVector"))
			{
				Json AnimModifiers = CJsonStorage::GetInstance()->FindOrLoadJson(s_ModifyFilePath);

				Json json;
				SaveModifiedData(json);
				AnimModifiers[m_strName] = json;

				CJsonStorage::GetInstance()->UpdateJson(s_ModifyFilePath, AnimModifiers);
				CJsonStorage::GetInstance()->SaveJson(s_ModifyFilePath);
			}

		}
	}
}

void CModel::SetPlayAnimation(const string& strAnimName)
{
	const auto itr = m_mapAnimation.find(strAnimName);
	if (itr != m_mapAnimation.end())
	{
		m_CurAnimName = strAnimName;
		itr->second->Reset();
	}
}

CAnimation* CModel::GetPlayAnimation()
{
	const auto itr = m_mapAnimation.find(m_CurAnimName);
	if (itr != m_mapAnimation.end())
		return itr->second;
	return nullptr;
}

void CModel::SetCurAnimName(const string & strAnimName)
{
	m_CurAnimName = strAnimName;
}

void CModel::Play_Animation(_double TimeDelta)
{
	if (m_eType == TYPE_NONANIM)
	{
		// IM_WARN("STATIC Model Cannot player animation.");
		return;
	}
	if (auto pAnim = Find_Animation(m_CurAnimName))
	{
		pAnim->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);

		KEYFRAME tempKeyFrame = *pAnim->GetCurKeyFrame();
		if (tempKeyFrame.Time != m_CurKeyFrame.Time)
		{
			m_BefKeyFrame = m_CurKeyFrame;
			m_CurKeyFrame = tempKeyFrame;
		}
	}

	Compute_CombindTransformationMatrix();
}

void CModel::Play_Animation_Test(_double TimeDelta)
{
	if (m_eType == TYPE_NONANIM)
	{
		IM_WARN("STATIC Model Cannot player animation.");
		return;
	}
	// m_Animations[m_iAnimIdx]->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
	//
	// SetBoneMask(EBoneMask::UPPER_BLOCK, "R_Shoulder");
	// SetBoneMask(EBoneMask::UPPER_BLOCK, "L_Shoulder");
	// m_Animations[m_iAdditiveAnimIdx]->Update_Bones(TimeDelta, EAnimUpdateType::ADDITIVE, m_fAdditiveRatio);
	// SetBoneMask(EBoneMask::MASK_OFF);
	//
	// Compute_CombindTransformationMatrix();
	//
	//
	// m_Animations[m_iHeadAnim]->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
	//
	// SetBoneMask(EBoneMask::OFF_CHILD_EQ, "IK_RootTarget");
	// SetBoneMask(EBoneMask::OFF_CHILD_EQ, "Neck");
	// m_Animations[m_iUB]->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
	//
	// SetBoneMask(EBoneMask::OFF_CHILD_EQ, "Spine1");
	// m_Animations[m_iLB]->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);
	//
	//
	// SetBoneMask(EBoneMask::ON_ALL);
	// SetBoneMask(EBoneMask::OFF_PARENT, "Pelvis");
	// SetBoneMask(EBoneMask::OFF_CHILD_EQ, "Neck");
	// m_Animations[m_iAdditiveAnimIdx]->Update_Bones(TimeDelta, EAnimUpdateType::ADDITIVE, m_fAdditiveRatio);
	// SetBoneMask(EBoneMask::ON_ALL);


	Compute_CombindTransformationMatrix();
}

void CModel::Play_Animation_Additive(_double TimeDelta)
{
	if (m_eType == TYPE_NONANIM)
	{
		// IM_WARN("STATIC Model Cannot player animation.");
		return;
	}

	if (auto pAnim = Find_Animation(m_CurAnimName))
	{
		pAnim->Update_Bones(TimeDelta, EAnimUpdateType::NORMAL);

		if ("" != m_szAdditiveAnimName)
		{
			if (auto pAnim = Find_Animation(m_szAdditiveAnimName))
			{
				pAnim->Update_Bones(TimeDelta, EAnimUpdateType::ADDITIVE, m_fAdditiveRatio);
			}
		}

		KEYFRAME tempKeyFrame = *pAnim->GetCurKeyFrame();
		if (tempKeyFrame.Time != m_CurKeyFrame.Time)
		{
			m_BefKeyFrame = m_CurKeyFrame;
			m_CurKeyFrame = tempKeyFrame;
		}
	}

	Compute_CombindTransformationMatrix();
}

// HRESULT CModel::RenderCustomShader(_uint iPass, CShader* pShader)
// {
// 	for (const auto& mesh : m_Meshes)
// 	{
// 		if (m_eType == TYPE_ANIM)
// 		{
// 			_float4x4 BoneMatrices[128];
// 			mesh->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
// 			pShader->Set_MatrixArray("g_BoneMatrices", BoneMatrices, 128);
// 		}
//
// 		pShader->Begin(iPass);
// 		mesh->Render();
// 	}
//
// 	return S_OK;
// }

HRESULT CModel::Render(CTransform* pTransform)
{
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		RenderMesh(pTransform, static_cast<_uint>(i));
	}

	return S_OK;
}

HRESULT CModel::Render(const _float4x4& WorldMatrix)
{
	for (const auto& mesh : m_Meshes)
	{
		const _uint iMtrlIdx = mesh->Get_MaterialIndex();
		if (m_Materials[iMtrlIdx]->IsActive() == false)
			continue;

		if (m_eType == TYPE_ANIM)
		{
			_float4x4 BoneMatrices[512];
			mesh->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
			m_Materials[iMtrlIdx]->GetShader()->Set_MatrixArray("g_BoneMatrices", BoneMatrices, 512);
		}

		m_Materials[iMtrlIdx]->BindMatrices(WorldMatrix);
		m_Materials[iMtrlIdx]->Begin();
		mesh->Render();
	}

	return S_OK;
}

HRESULT CModel::RenderMesh(CTransform* pTransform, _uint iMeshIdx)
{
	const _uint iMtrlIdx = m_Meshes[iMeshIdx]->Get_MaterialIndex();
	if (m_Materials[iMtrlIdx]->IsActive() == false)
		return S_OK;

	if (m_eType == TYPE_ANIM)
	{
		_float4x4 BoneMatrices[512];
		m_Meshes[iMeshIdx]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
		m_Materials[iMtrlIdx]->GetShader()->Set_MatrixArray("g_BoneMatrices", BoneMatrices, 512);
	}

	m_Materials[iMtrlIdx]->BindMatrices(pTransform);
	m_Materials[iMtrlIdx]->Begin();
	return m_Meshes[iMeshIdx]->Render();
}

HRESULT CModel::RenderMeshOnly(_uint iMeshIdx) const
{
	return m_Meshes[iMeshIdx]->Render();
}

HRESULT CModel::Render_Pass(CTransform* pTransform, _uint iPass)
{
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		const _uint iMtrlIdx = m_Meshes[i]->Get_MaterialIndex();
		if (m_Materials[iMtrlIdx]->IsActive() == false)
			continue;

		if (m_eType == TYPE_ANIM)
		{
			_float4x4 BoneMatrices[512];
			m_Meshes[i]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
			m_Materials[iMtrlIdx]->GetShader()->Set_MatrixArray("g_BoneMatrices", BoneMatrices, 512);
		}

		m_Materials[iMtrlIdx]->BindMatrices(pTransform);
		m_Materials[iMtrlIdx]->Begin(iPass);
		FAILED_CHECK(m_Meshes[i]->Render());
	}

	return S_OK;
}

HRESULT CModel::Render_Cam(CTransform* pTransform, CCamera* pCamera)
{
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		const _uint iMtrlIdx = m_Meshes[i]->Get_MaterialIndex();
		if (m_Materials[iMtrlIdx]->IsActive() == false)
			continue;

		if (m_eType == TYPE_ANIM)
		{
			_float4x4 BoneMatrices[512];
			m_Meshes[i]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
			m_Materials[iMtrlIdx]->GetShader()->Set_MatrixArray("g_BoneMatrices", BoneMatrices, 512);
		}

		m_Materials[iMtrlIdx]->BindMatrices(pTransform, pCamera);
		m_Materials[iMtrlIdx]->Begin();
		FAILED_CHECK(m_Meshes[i]->Render());
	}

	return S_OK;
}

// HRESULT CModel::RenderMesh_Shader(CTransform* pTransform, CShader* pShader, _uint iPass)
// {
//
// }

HRESULT CModel::Render_ShadowDepth(CTransform* pTransform)
{
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		const _uint iMtrlIdx = m_Meshes[i]->Get_MaterialIndex();
		if (m_Materials[iMtrlIdx]->IsActive() == false)
			continue;

		if (m_eType == TYPE_ANIM)
		{
			_float4x4 BoneMatrices[512];
			m_Meshes[i]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
			m_pShadowShader->Set_MatrixArray("g_BoneMatrices", BoneMatrices, 512);
		}
		
		FAILED_CHECK(pTransform->Bind_ShaderResource(m_pShadowShader, "g_WorldMatrix"));

		FAILED_CHECK(m_pShadowShader->Set_Matrix("g_ViewMatrix", CLight_Manager::GetInstance()->GetShadowLightView()));
		FAILED_CHECK(m_pShadowShader->Set_Matrix("g_ProjMatrix", CLight_Manager::GetInstance()->GetShadowLightProj()));

		m_pShadowShader->Begin(0);
		m_Meshes[i]->Render();
	}

	return S_OK;
}

HRESULT CModel::Render_ShadowDepth(const _float4x4& WorldMatrix)
{
	for (size_t i = 0; i < m_Meshes.size(); ++i)
	{
		const _uint iMtrlIdx = m_Meshes[i]->Get_MaterialIndex();
		if (m_Materials[iMtrlIdx]->IsActive() == false)
			continue;

		if (m_eType == TYPE_ANIM)
		{
			_float4x4 BoneMatrices[512];
			m_Meshes[i]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));
			m_pShadowShader->Set_MatrixArray("g_BoneMatrices", BoneMatrices, 512);
		}

		FAILED_CHECK(m_pShadowShader->Set_Matrix("g_WorldMatrix", &WorldMatrix));
		FAILED_CHECK(m_pShadowShader->Set_Matrix("g_ViewMatrix", CLight_Manager::GetInstance()->GetShadowLightView()));
		FAILED_CHECK(m_pShadowShader->Set_Matrix("g_ProjMatrix", CLight_Manager::GetInstance()->GetShadowLightProj()));

		m_pShadowShader->Begin(0);
		m_Meshes[i]->Render();
	}

	return S_OK;
}

void CModel::LoadAnimations(const char* pAnimDir)
{
	Assert(m_eType == TYPE_ANIM);

	CGameUtils::ListFiles(pAnimDir, [this](const string& pAnimPath)
	{
		auto pAnim = CAnimation::Create(pAnimPath.c_str());
		m_mapAnimation.emplace(pAnim->GetName(), pAnim);
	});

	int iA = 0;
}

void CModel::AddBoneRotation(const string& strBoneName, Quaternion qQuat)
{
	CBone* pBone = Get_BonePtr(strBoneName);
	Assert(pBone != nullptr);

	
	// pBone->Set_TransformMatrix(XMMatrixRotationQuaternion(qQuat) * XMLoadFloat4x4(&pBone->Get_TransformMatrix()));
	pBone->Set_TransformMatrix(XMMatrixRotationQuaternion(qQuat) * XMLoadFloat4x4(&pBone->Get_TransformMatrix()));
}

void CModel::SetBoneMask(EBoneMask eMask, const string& strBoneName)
{
	CBone* pStartBone = Get_BonePtr(strBoneName);

	switch (eMask)
	{
	case EBoneMask::OFF_PARENT_EQ:
		Assert(pStartBone != nullptr);
		pStartBone->DisableToParentRecursive(true);
		break;
	case EBoneMask::OFF_PARENT:
		Assert(pStartBone != nullptr);
		pStartBone->DisableToParentRecursive(true);
		pStartBone->SetDisableAnimation(false);
		break;
	case EBoneMask::OFF_CHILD_EQ:
		Assert(pStartBone != nullptr);
		pStartBone->DisableToChildRecursive(true);
		break;
	case EBoneMask::OFF_CHILD:
		Assert(pStartBone != nullptr);
		pStartBone->DisableToChildRecursive(true);
		pStartBone->SetDisableAnimation(false);
		break;
	case EBoneMask::OFF_EQ:
		Assert(pStartBone != nullptr);
		pStartBone->SetDisableAnimation(true);
		break;
	case EBoneMask::OFF_ALL:
		for (const auto& pBone : m_mapBones)
			pBone.second->SetDisableAnimation(true);
		break;
	case EBoneMask::ON_PARENT_EQ:
		Assert(pStartBone != nullptr);
		pStartBone->DisableToParentRecursive(false);
		break;
	case EBoneMask::ON_PARENT:
		Assert(pStartBone != nullptr);
		pStartBone->DisableToParentRecursive(false);
		pStartBone->SetDisableAnimation(true);
		break;
	case EBoneMask::ON_CHILD_EQ:
		Assert(pStartBone != nullptr);
		pStartBone->DisableToChildRecursive(false);
		break;
	case EBoneMask::ON_CHILD:
		Assert(pStartBone != nullptr);
		pStartBone->DisableToChildRecursive(false);
		pStartBone->SetDisableAnimation(true);
		break;
	case EBoneMask::ON_EQ:
		Assert(pStartBone != nullptr);
		pStartBone->SetDisableAnimation(false);
		break;
	case EBoneMask::ON_ALL:
		for (const auto& pBone : m_mapBones)
			pBone.second->SetDisableAnimation(false);
		break;
	case EBoneMask::BONE_MASK_END:
		FALLTHROUGH
	default: ;
		NODEFAULT
	}
}

void CModel::Compute_CombindTransformationMatrix()
{
	m_pRootBone->Compute_CombindTransformationMatrix_StartRoot();
}		

CMaterial* CModel::FindMaterial(const _tchar* pMtrlProtoTag)
{
	if (pMtrlProtoTag == nullptr)
		return nullptr;

	for (auto pMtrl : m_Materials)
	{
		if (lstrcmp(pMtrl->GetPrototypeTag(), pMtrlProtoTag) == 0)
			return pMtrl;
	}

	return nullptr;
}

void CModel::SaveModifiedData(Json & json)
{
	json["mapRootMotion"] = m_mapOptionalRootMotion;
}

void CModel::Ready_Bones(const Json& jBone, CBone* pParent)
{
	CBone*		pBone = nullptr;

	if (m_bPx)
		pBone = CPxBone::Create(jBone, pParent);
	else
		pBone = CBone::Create(jBone, pParent);

	Assert(pBone != nullptr);

	if (pParent == nullptr)
	{
		m_pRootBone = pBone;
		Safe_AddRef(m_pRootBone);
	}

	// ���� ������ �ִ°�, ������ ���� addref���� �ʴ´�.
	m_mapBones.emplace(jBone["Name"].get<string>(), pBone);

	if (jBone.contains("Children"))
	{
		for (auto& jBoneChild : jBone["Children"])
		{
			Ready_Bones(jBoneChild, pBone);
		}
	}
}

void CModel::SetBoneChildren(const Json& jBone)
{
	const string boneName = jBone["Name"].get<string>();
	CBone* pBone = Get_BonePtr(boneName.c_str());

	if (jBone.contains("Children"))
	{
		for (auto& jBoneChild : jBone["Children"])
		{
			const string childBoneName = jBoneChild["Name"].get<string>();
			CBone* pChildBone = Get_BonePtr(childBoneName.c_str());
			pBone->AddChildBone(pChildBone);
		}

		for (auto& jBoneChild : jBone["Children"])
		{
			SetBoneChildren(jBoneChild);
		}
	}
}

HRESULT CModel::Ready_Materials(HANDLE hFile)
{
	DWORD dwByte = 0;

	_uint iNumMaterials = 0;
	ReadFile(hFile, &iNumMaterials, sizeof(_uint), &dwByte, nullptr); /* Read */

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		const string mtrlName = CGameUtils::ReadStrFile(hFile); /* Read */
		wstring wMtrlName = CGameUtils::s2ws(mtrlName);

		if (nullptr == CGameInstance::GetInstance()->Find_Prototype_Component(LEVEL_NOW, wMtrlName.c_str()))
		{
			const string* pMtrlPath = CMaterial::FindMaterialFilePath(mtrlName);

			if (pMtrlPath)
				CGameInstance::GetInstance()->Add_Prototype(wMtrlName.c_str(), CMaterial::Create(m_pDevice, m_pContext, pMtrlPath->c_str()));
		}

		CMaterial* pMtrl = dynamic_cast<CMaterial*>(CGameInstance::GetInstance()->Clone_Component(wMtrlName.c_str()));
	
		if (pMtrl == nullptr)
		{
			if (m_eType == TYPE_NONANIM)
			{
				pMtrl = dynamic_cast<CMaterial*>(CGameInstance::GetInstance()->Clone_Component(L"Proto_Mtrl_Empty"));
			}
			else if (m_eType == TYPE_ANIM)
			{
				pMtrl = dynamic_cast<CMaterial*>(CGameInstance::GetInstance()->Clone_Component(L"Proto_MtrlAnim_Empty"));
			}
			IM_WARN("Fail to Find Mtrl : %s", mtrlName.c_str());
		}
	
		pMtrl->SetAnimType(m_eType);
		m_Materials.push_back(pMtrl);
	}

	return S_OK;
}

void CModel::EventCaller(const string& EventName)
{
	for (auto& iter : m_EventFunc)
	{
		if (EventName == iter.first) iter.second();
	}
}

void CModel::Add_EventCaller(const string & EventName, std::function<void(void)> Func)
{
	m_EventFunc.emplace(EventName, Func);
}

_vector CModel::GetOptionalMoveVector(_fmatrix WorldMatrix)
{
	static _vector vInitTrans;
	static _float fStartTime;
	static string szCurAnimName;

	if (szCurAnimName != m_CurAnimName)
	{
		szCurAnimName = m_CurAnimName;
		vInitTrans = XMVectorSet(0.f, 0.f, 0.f, -1.f);
	}

	for (auto& iter : m_mapOptionalRootMotion)
	{
		if (m_CurAnimName == iter.first)	// �̺�Ʈ �ݷ��� ���� ����
		{
			_float fPlayTime = static_cast<_float>(m_mapAnimation[m_CurAnimName]->GetPlayTime());

			for (auto& Optional : iter.second)
			{
				_float fRatio = (fPlayTime - Optional.fStartTime) / (Optional.fEndTime - Optional.fStartTime);

				if (0.f <= fRatio && 1.f >= fRatio)
				{
					_vector vScale, vRotation, vTrans;
					XMMatrixDecompose(&vScale, &vRotation, &vTrans, WorldMatrix);
					_matrix WorldRotation = XMMatrixRotationQuaternion(vRotation);

					if (-1.f == XMVectorGetW(vInitTrans))
					{
						vInitTrans = vTrans;
					}

					if (fStartTime != Optional.fStartTime)
					{
						fStartTime = Optional.fStartTime;
						vInitTrans = vTrans;
					}

					/*if (szCurAnimName != m_CurAnimName)
					{
						szCurAnimName = m_CurAnimName;
						vInitTrans = vTrans;
					}*/

					_vector vMovePos = Optional.vOptionalRootVector;
					vMovePos = XMVectorSetW(vMovePos, 0.f);

					vMovePos = XMVector3TransformNormal(vMovePos, WorldRotation);

					_vector vDestPos = vInitTrans + vMovePos;

					vDestPos = XMVectorLerp(vInitTrans, vDestPos, fRatio);

					_vector vResultDir = vDestPos - vTrans;
					vResultDir = XMVectorSetW(vResultDir, 0.f);

					if (0.f > XMVectorGetX(XMVector3Dot(vResultDir, vMovePos)))
					{
						// Ȥ�� ���⺤�� Ƣ�� ��� �����
						vInitTrans = vTrans;
						GetOptionalMoveVector(WorldMatrix);
					}

					return vResultDir;
				}
			}
		}
	}

	vInitTrans = XMVectorSet(0.f, 0.f, 0.f, -1.f);
	return XMVectorSet(0.f, 0.f, 0.f, 0.f);
}

_vector CModel::GetOptionalMoveVector(_fmatrix WorldMatrix, const string & srtAnimName)
{
	static _vector vInitTrans;
	static _float fStartTime;
	static string szCurAnimName;

	if (szCurAnimName != srtAnimName)
	{
		szCurAnimName = srtAnimName;
		vInitTrans = XMVectorSet(0.f, 0.f, 0.f, -1.f);
	}

	for (auto& iter : m_mapOptionalRootMotion)
	{
		if (srtAnimName == iter.first)	// �̺�Ʈ �ݷ��� ���� ����
		{
			_float fPlayTime = static_cast<_float>(m_mapAnimation[srtAnimName]->GetPlayTime());

			for (auto& Optional : iter.second)
			{
				_float fRatio = (fPlayTime - Optional.fStartTime) / (Optional.fEndTime - Optional.fStartTime);

				if (0.f <= fRatio && 1.f >= fRatio)
				{
					_vector vScale, vRotation, vTrans;
					XMMatrixDecompose(&vScale, &vRotation, &vTrans, WorldMatrix);
					_matrix WorldRotation = XMMatrixRotationQuaternion(vRotation);

					if (-1.f == XMVectorGetW(vInitTrans))
					{
						vInitTrans = vTrans;
					}

					if (fStartTime != Optional.fStartTime)
					{
						fStartTime = Optional.fStartTime;
						vInitTrans = vTrans;
					}

					_vector vMovePos = Optional.vOptionalRootVector;
					vMovePos = XMVectorSetW(vMovePos, 0.f);

					vMovePos = XMVector3TransformNormal(vMovePos, WorldRotation);

					_vector vDestPos = vInitTrans + vMovePos;

					vDestPos = XMVectorLerp(vInitTrans, vDestPos, fRatio);

					_vector vResultDir = vDestPos - vTrans;
					vResultDir = XMVectorSetW(vResultDir, 0.f);

					if (0.f > XMVectorGetX(XMVector3Dot(vResultDir, vMovePos)))
					{
						// Ȥ�� ���⺤�� Ƣ�� ��� �����
						vInitTrans = vTrans;
						GetOptionalMoveVector(WorldMatrix);
					}

					return vResultDir;
				}
			}
		}
	}

	vInitTrans = XMVectorSet(0.f, 0.f, 0.f, -1.f);
	return XMVectorSet(0.f, 0.f, 0.f, 0.f);
}

void CModel::Add_OptionalRootMotion(OPTIONAL_ROOTMOTION RootMotion)
{
	RootMotion.szAnimName = m_CurAnimName;

	const auto iter = m_mapOptionalRootMotion.find(m_CurAnimName);
	if (iter == m_mapOptionalRootMotion.end())
	{
		vector<OPTIONAL_ROOTMOTION> vecRoot;
		m_mapOptionalRootMotion.emplace(m_CurAnimName, vecRoot);
		m_mapOptionalRootMotion[m_CurAnimName].push_back(RootMotion);
	}
	else
	{
		iter->second.push_back(RootMotion);
	}
}

void CModel::Delete_OptionalRootMotion()
{
	m_mapOptionalRootMotion.erase(m_CurAnimName);
}

CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const char * pModelFilePath, _float4x4 PivotMatrix)
{
	CModel*		pInstance = new CModel(pDevice, pContext);
	pInstance->m_PivotMatrix = PivotMatrix;

	if (FAILED(pInstance->Initialize_Prototype(pModelFilePath)))
	{
		MSG_BOX("Failed to Created : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CModel::Clone(void * pArg)
{
	CModel*		pInstance = new CModel(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	for (auto& pMesh : m_Meshes)
		Safe_Release(pMesh);
	m_Meshes.clear();

	for (auto& BonePair : m_mapBones)
		Safe_Release(BonePair.second);
	m_mapBones.clear();

	for (auto& pMtrl : m_Materials)
		Safe_Release(pMtrl);
	m_Materials.clear();

	for (auto pAnimPair : m_mapAnimation)
		Safe_Release(pAnimPair.second);
	m_mapAnimation.clear();

	Safe_Release(m_pRootBone);

	Safe_Release(m_pShadowShader);
}
