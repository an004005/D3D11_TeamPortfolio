#include "stdafx.h"
#include "Navigation.h"
#include "Cell.h"
#include "Shader.h"
#include "GameInstance.h"
#include "GameUtils.h"
#include "JsonLib.h"

CNavigation::CNavigation(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CNavigation::CNavigation(const CNavigation & rhs)
	: CGameObject(rhs)
#ifdef _DEBUG
	, m_pShader(rhs.m_pShader)
#endif // _DEBUG
{
#ifdef _DEBUG
	Safe_AddRef(m_pShader);
#endif // _DEBUG
}

HRESULT CNavigation::Initialize_Prototype()
{
#ifdef _DEBUG
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Navigation.hlsl"), VTXPOS_DECLARATION::Elements, VTXPOS_DECLARATION::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CNavigation::Initialize(void * pArg)
{
	if (nullptr != pArg)
	{
		LoadJson(*static_cast<Json*>(pArg));
	}
	m_bVisible = false;

#ifdef _DEBUG
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom))
#endif


	return S_OK;
}

void CNavigation::SaveToJson(Json& json)
{
	json["Cells"] = Json::array();

	for (auto pCell : m_Cells)
	{
		Json cell;
		cell["A"] = pCell->Get_Point(CCell::POINT_A);
		cell["B"] = pCell->Get_Point(CCell::POINT_B);
		cell["C"] = pCell->Get_Point(CCell::POINT_C);
		cell["bCliff"] = pCell->IsCliff();
		json["Cells"].push_back(cell);
	}
}

void CNavigation::LoadJson(const Json& json)
{
	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
	m_Cells.clear();

	_float3 vCellPoses[3];

	m_Cells.reserve(json["Cells"].size());
	for (auto& jsonCell : json["Cells"])
	{
		vCellPoses[CCell::POINT_A] = jsonCell["A"];
		vCellPoses[CCell::POINT_B] = jsonCell["B"];
		vCellPoses[CCell::POINT_C] = jsonCell["C"];
		CCell*		pCell = CCell::Create(m_pDevice, m_pContext, vCellPoses, (_int)m_Cells.size());
		pCell->SetCliff(jsonCell["bCliff"]);

		m_Cells.push_back(pCell);
	}

	Ready_Neighbor();
}

#ifdef _DEBUG

void CNavigation::Late_Tick(_double TimeDelta)
{
	if (m_bVisible)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DEBUG, this);
}

HRESULT CNavigation::Render()
{
	_float		fHeight = 0.0f;

	fHeight = 0.1f;
	for (const auto iCheckedIdx : m_CheckedIdxes)
	{
		if (iCheckedIdx >= m_Cells.size())
			continue;

		HRESULT hr = m_pShader->Set_RawValue("g_fHeight", &fHeight, sizeof(_float));
		m_pShader->Set_RawValue("g_vColor", &_float4(1.f, 0.f, 0.f, 1.f), sizeof(_float4));

		m_Cells[iCheckedIdx]->Render(m_pShader);
	}

	for (auto& pCell : m_Cells)
	{
		if (pCell->IsCliff())
		{
			fHeight = 0.05f;
			HRESULT hr = m_pShader->Set_RawValue("g_fHeight", &fHeight, sizeof(_float));
			m_pShader->Set_RawValue("g_vColor", &_float4(0.f, 0.f, 1.f, 1.f), sizeof(_float4));
			pCell->Render(m_pShader);
		}

		fHeight = 0.0f;
		HRESULT hr = m_pShader->Set_RawValue("g_fHeight", &fHeight, sizeof(_float));
		m_pShader->Set_RawValue("g_vColor", &_float4(0.f, 1.f, 0.f, 1.f), sizeof(_float4));
		pCell->Render(m_pShader);
	}

	m_CheckedIdxes.clear();

	return S_OK;
}

void CNavigation::AddCell(const array<_float3, 3>& CellPos)
{
	CCell*		pCell = CCell::Create(m_pDevice, m_pContext, CellPos.data(), (_int)m_Cells.size());
	m_Cells.push_back(pCell);
	Ready_Neighbor();
}

_bool CNavigation::FindNearCellPoint(_float3& vPos)
{
	_float4 vRayOrigin;
	_float4 vRayDir;

	CGameUtils::GetPickingRay(vRayOrigin, vRayDir);
	vRayDir.Normalize();

	const _vector _vRayOrigin = XMLoadFloat4(&vRayOrigin);
	const _vector _vRayDir = XMLoadFloat4(&vRayDir);

	BoundingSphere Sphere;
	Sphere.Radius = 0.5f;

	_float fDistance = 0.f;
	for (auto pCell : m_Cells)
	{
		for (int i = 0; i < CCell::POINT_END; ++i)
		{
			Sphere.Center = pCell->Get_Point(static_cast<CCell::POINT>(i));

			if (Sphere.Intersects(_vRayOrigin, _vRayDir, fDistance))
			{
				vPos = Sphere.Center;
				return true;
			}
		}
	}

	return false;
}

CCell* CNavigation::PickCell()
{
	_float4 vRayOrigin;
	_float4 vRayDir;

	CGameUtils::GetPickingRay(vRayOrigin, vRayDir);
	vRayDir.Normalize();

	const _vector _vRayOrigin = XMLoadFloat4(&vRayOrigin);
	const _vector _vRayDir = XMLoadFloat4(&vRayDir);

	_float fDistance;
	for (auto pCell : m_Cells)
	{
		if (TriangleTests::Intersects(_vRayOrigin, _vRayDir, 
			pCell->Get_Point(CCell::POINT_A), 
			pCell->Get_Point(CCell::POINT_B), 
			pCell->Get_Point(CCell::POINT_C),
			fDistance))
		{
			return pCell;
		}
	}

	return nullptr;
}

void CNavigation::DeleteCell(_uint iCellIdx)
{
	Safe_Release(m_Cells[iCellIdx]);
	m_Cells.erase(m_Cells.begin() + iCellIdx);
}

void CNavigation::DeleteRecentCell()
{
	Safe_Release(m_Cells.back());
	m_Cells.pop_back();
}

void CNavigation::GetAllNearCellPoints(list<pair<CCell*, CCell::POINT>>& CellPointPairs)
{
	_float4 vRayOrigin;
	_float4 vRayDir;

	CGameUtils::GetPickingRay(vRayOrigin, vRayDir);
	vRayDir.Normalize();

	const _vector _vRayOrigin = XMLoadFloat4(&vRayOrigin);
	const _vector _vRayDir = XMLoadFloat4(&vRayDir);

	BoundingSphere Sphere;
	Sphere.Radius = 0.5f;

	_float fDistance = 0.f;
	for (auto pCell : m_Cells)
	{
		for (int i = 0; i < CCell::POINT_END; ++i)
		{
			Sphere.Center = pCell->Get_Point(static_cast<CCell::POINT>(i));

			if (Sphere.Intersects(_vRayOrigin, _vRayDir, fDistance))
			{
				CellPointPairs.push_back({pCell, static_cast<CCell::POINT>(i)});
			}
		}
	}
}

#endif // _DEBUG

_bool CNavigation::IsMove_OnNavigation(_int& iCellIdx_InOut, _fvector TargetPos, _float3& vFailedLineVector)
{
	if (-1 == iCellIdx_InOut)
		return false;

	_int		iNeighborIndex = -1;

	/* 움직이고 난 결과위치가 쎌 안에 있다면.  */
	if (true == m_Cells[iCellIdx_InOut]->IsIn(TargetPos, &iNeighborIndex))
	{
#ifdef _DEBUG
		m_CheckedIdxes.push_back(iCellIdx_InOut);
#endif
		return true;
	}
	else
	{
		/* 나간방향으로 이웃이 있었다면ㄴ. */
		if (-1 != iNeighborIndex)
		{
			_int iLoopBreaker = 300;
			while (iLoopBreaker--)
			{
				if (-1 == iNeighborIndex)
				{
					vFailedLineVector = m_Cells[iCellIdx_InOut]->GetOutLineVector(TargetPos);
					return false;
				}

				if (true == m_Cells[iNeighborIndex]->IsIn(TargetPos, &iNeighborIndex))
				{
					// m_NaviDesc.iCurrentIndex = 이웃의 인덱스;
					iCellIdx_InOut = iNeighborIndex;
#ifdef _DEBUG
					m_CheckedIdxes.push_back(iCellIdx_InOut);
#endif
					return true;
				}				
			}

			vFailedLineVector = m_Cells[iCellIdx_InOut]->GetOutLineVector(TargetPos);
			return false;
		}
		/* 나간방향으로 이웃이 없었다면 */
		else
		{
			vFailedLineVector = m_Cells[iCellIdx_InOut]->GetOutLineVector(TargetPos);
			return false;
		}
	}
}

_float3 CNavigation::GetCellPos(_int iCellIdx)
{
	Assert(iCellIdx >= 0);
	Assert(iCellIdx < m_Cells.size());

	_vector vPosSum = m_Cells[iCellIdx]->Get_Point(CCell::POINT_A);
	vPosSum += m_Cells[iCellIdx]->Get_Point(CCell::POINT_B);
	vPosSum += m_Cells[iCellIdx]->Get_Point(CCell::POINT_C);
	vPosSum /= 3.f;

	return vPosSum;
}

_int CNavigation::GetCellIdx(_float4 vPos) const
{
	_vector vOrigin = vPos;

	for (size_t i = 0; i < m_Cells.size(); ++i)
	{
		if (m_Cells[i]->GetHeight(vOrigin) > -1000.f)
		{
			return static_cast<_int>(i);
		}
	}

	return 0;
}

_float CNavigation::GetCellHeight(_fvector vPos, _int iCellIdx)
{
	// 실패시 -1000 리턴
	_float fHeight = m_Cells[iCellIdx]->GetHeight(vPos);

	if (fHeight <= -1000.f)
	{
		for (int i = 0; i < CCell::NEIGHBOR_END; ++i)
		{
			const _int iNeighbor = m_Cells[iCellIdx]->GetNeighborIdx(static_cast<CCell::NEIGHBOR>(i));
			if (iNeighbor >= 0)
			{
				fHeight = m_Cells[iNeighbor]->GetHeight(vPos);
				if (fHeight > -1000.f)
					return fHeight;
			}
		}
	}

	return fHeight;
}

_vector CNavigation::GetCellNormal(_int iCellIdx)
{
	Assert(iCellIdx < m_Cells.size());
	return m_Cells[iCellIdx]->GetNormal();
}

/* 네비게이션을 구성하는 쎌들의 이웃을 설정한다. */
HRESULT CNavigation::Ready_Neighbor()
{
	for (auto& pSourCell : m_Cells)
	{
		for (auto& pDestCell : m_Cells)
		{
			if (pSourCell == pDestCell)
				continue;

			if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
			{
				pSourCell->Set_Neighbor(CCell::NEIGHBOR_AB, pDestCell);
			}

			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
			{
				pSourCell->Set_Neighbor(CCell::NEIGHBOR_BC, pDestCell);
			}

			else if (true == pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
			{
				pSourCell->Set_Neighbor(CCell::NEIGHBOR_CA, pDestCell);
			}
		}
	}
	
	return S_OK;
}

CNavigation * CNavigation::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CNavigation*		pInstance = new CNavigation(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CNavigation::Clone(void * pArg)
{
	CNavigation*		pInstance = new CNavigation(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CNavigation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNavigation::Free()
{
	__super::Free();

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);
	m_Cells.clear();

#ifdef _DEBUG
	Safe_Release(m_pShader);
	Safe_Release(m_pRendererCom);
#endif // _DEBUG
}
