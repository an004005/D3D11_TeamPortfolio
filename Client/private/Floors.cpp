#include "stdafx.h"
#include "..\public\Floors.h"
#include "GameInstance.h"
#include "JsonLib.h"
#include "VIBuffer_PosRect.h"
#include "GameUtils.h"
#include "ImguiUtils.h"
#include "MathUtils.h"

const array<_float3, 4> CFloors::s_RectLocalPos{
	_float3{-0.5f, 0.f, 0.5f}, // left top
	_float3{0.5f, 0.f, 0.5f}, // right top
	_float3{0.5f, 0.f, -0.5f}, // right bot
	_float3{-0.5f, 0.f, -0.5f} // left bot
};

CFloors::CFloors(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CFloors::CFloors(const CFloors& rhs)
	: CGameObject(rhs)
{
}

HRESULT CFloors::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFloors::Initialize(void* pArg)
{
	FAILED_CHECK(__super::Initialize(pArg));

	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	/* For.Com_Renderer */
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom))

	/* For.Com_Shader */
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_Shader_VtxPosRect"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom))

	/* For.Com_VIBuffer */
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW, TEXT("Prototype_Component_VIBuffer_PosRect"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom))

	if (pArg)
	{
		Json& json = *static_cast<Json*>(pArg);
		LoadJson(json);
	}

	return S_OK;
}

void CFloors::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

}

void CFloors::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CFloors::Render()
{
	static const _float4 selectColor{1.f, 0.f, 0.f, 1.f};
	static const _float4 defaultColor{1.f, 1.f, 1.f, 1.f};

	FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW)))
	FAILED_CHECK(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ)))

	for (size_t i = 0; i < m_Floors.size(); ++i)
	{
		FAILED_CHECK(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_Floors[i].WorldMatrix));
		FAILED_CHECK(m_pShaderCom->Set_RawValue("g_color", 
			i == static_cast<size_t>(m_iSelectedFloor) ? &selectColor : &defaultColor, sizeof(_float4)));

		m_pShaderCom->Begin(0);
		m_pVIBufferCom->Render();
	}

	return S_OK;
}

void CFloors::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();

	static _bool bLockPickFloor = false;

	if (ImGui::Button("Create Floor"))
	{
		CreateFloor();
	}

	if (ImGui::Button("Delete Selected"))
	{
		if (!m_Floors.empty())
			m_Floors.erase(m_Floors.begin() + m_iSelectedFloor);
		m_iSelectedFloor = 0;
	}

	int iFloorIdx = static_cast<int>(m_iSelectedFloor);
	ImGui::InputInt("Select Floor Idx", &iFloorIdx);
	if (iFloorIdx >= 0 && iFloorIdx < static_cast<int>(m_Floors.size()))
	{
		m_iSelectedFloor = static_cast<_uint>(iFloorIdx);
	}


	if (bLockPickFloor == false && m_pGameInstance->KeyUp(CInput_Device::DIM_LB))
	{
		PickFloor();
	}

	if (bLockPickFloor)
		ImGui::Text("Picking Locked");
	else
		ImGui::Text("Picking Opened");

	if (ImGui::Button("Lock Picking"))
	{
		bLockPickFloor = !bLockPickFloor;
	}

	if (!m_Floors.empty())
	{
		static GUIZMO_INFO tInfo;
		CImguiUtils::Render_Guizmo(&m_Floors[m_iSelectedFloor].WorldMatrix, tInfo, true, true);
		SetFloorPoint(m_Floors[m_iSelectedFloor]);
	}
}

void CFloors::SaveToJson(Json& json)
{
	__super::SaveToJson(json);

	json["Floors"] = Json::array();
	for (auto& floor : m_Floors)
	{
		json["Floors"].push_back(floor.WorldMatrix);
	}
}

void CFloors::LoadJson(const Json& json)
{
	m_Floors.clear();

	m_Floors.reserve(json["Floors"].size());
	for (const auto& jMat : json["Floors"])
	{
		FLOOR floor;
		floor.WorldMatrix = jMat;
		SetFloorPoint(floor);
		m_Floors.push_back(floor);
	}
}

_float CFloors::GetHeight(_float4 vPos) const
{
	_float fMaxHeight = -10000.f;
	for (auto& floor : m_Floors)
	{
		if (CheckOnFloor(vPos, floor))
		{
			const _float fHeight = GetHeightOnFloor(vPos, floor);
			if (fHeight > fMaxHeight)
				fMaxHeight = fHeight;
		}
	}

	if (fMaxHeight == -10000.f) return vPos.y;
	return fMaxHeight;
}

void CFloors::CreateFloor()
{
	m_Floors.push_back({_float4x4::Identity, 
	{
		s_RectLocalPos[0],
		s_RectLocalPos[1],
		s_RectLocalPos[2],
		s_RectLocalPos[3]
	}});

	if (m_Floors.empty())
		m_iSelectedFloor = 0;
	else
		m_iSelectedFloor = static_cast<_uint>(m_Floors.size()) - 1;
}

void CFloors::PickFloor()
{
	_float4 vRayOrigin;
	_float4 vRayDir;
	CGameUtils::GetPickingRay(vRayOrigin, vRayDir);

	const _vector _vRayOrigin = XMLoadFloat4(&vRayOrigin);
	const _vector _vRayDir = XMLoadFloat4(&vRayDir);

	_float fDistance = 0.f;
	for (size_t i = 0; i < m_Floors.size(); ++i)
	{
		const _vector v0 = XMLoadFloat3(&m_Floors[i].vRectPointPos[0]);
		const _vector v1 = XMLoadFloat3(&m_Floors[i].vRectPointPos[1]);
		const _vector v2 = XMLoadFloat3(&m_Floors[i].vRectPointPos[2]);

		if (TriangleTests::Intersects(_vRayOrigin, _vRayDir, v0, v1, v2, fDistance))
		{
			m_iSelectedFloor = static_cast<_uint>(i);
			return;
		}

		const _vector v3 = XMLoadFloat3(&m_Floors[i].vRectPointPos[3]);
		if (TriangleTests::Intersects(_vRayOrigin, _vRayDir, v0, v2, v3, fDistance))
		{
			m_iSelectedFloor = static_cast<_uint>(i);
			return;
		}
	}
}

_bool CFloors::PickFloorGetPos(_float3& vPos)
{
	_float4 vRayOrigin;
	_float4 vRayDir;

	CGameUtils::GetPickingRay(vRayOrigin, vRayDir);
	vRayDir.Normalize();

	const _vector _vRayOrigin = XMLoadFloat4(&vRayOrigin);
	const _vector _vRayDir = XMLoadFloat4(&vRayDir);
	

	_float fMinDist = FLT_MAX;

	_float fDistance = 0.f;
	for (size_t i = 0; i < m_Floors.size(); ++i)
	{
		const _vector v0 = XMLoadFloat3(&m_Floors[i].vRectPointPos[0]);
		const _vector v1 = XMLoadFloat3(&m_Floors[i].vRectPointPos[1]);
		const _vector v2 = XMLoadFloat3(&m_Floors[i].vRectPointPos[2]);

		if (TriangleTests::Intersects(_vRayOrigin, _vRayDir, v0, v1, v2, fDistance))
		{
			if (fMinDist > fDistance)
				fMinDist = fDistance;
		}

		const _vector v3 = XMLoadFloat3(&m_Floors[i].vRectPointPos[3]);
		if (TriangleTests::Intersects(_vRayOrigin, _vRayDir, v0, v2, v3, fDistance))
		{
			if (fMinDist > fDistance)
				fMinDist = fDistance;
		}
	}

	if (CMathUtils::FloatCmp(fMinDist, FLT_MAX))
	{
		return false;
	}
	else
	{
		vPos = _vRayOrigin + _vRayDir * fMinDist;
		return true;
	}
}

void CFloors::SetFloorPoint(FLOOR& tFloor)
{
	const _matrix WorldMatrix = XMLoadFloat4x4(&tFloor.WorldMatrix);
	for (int i = 0; i < 4; ++i)
	{
		_vector vPos = XMLoadFloat3(&s_RectLocalPos[i]);
		vPos = XMVector3TransformCoord(vPos, WorldMatrix);
		XMStoreFloat3(&tFloor.vRectPointPos[i], vPos);
	}
}

_bool CFloors::CheckOnFloor(const _float4& vPos, const FLOOR& tLastFloor) const
{
	return tLastFloor.vRectPointPos[0].x <= vPos.x && tLastFloor.vRectPointPos[0].z >= vPos.z // left top
		&& tLastFloor.vRectPointPos[2].x >= vPos.x && tLastFloor.vRectPointPos[2].z <= vPos.z; // right bot
}

_float CFloors::GetHeightOnFloor(const _float4& vPos, const FLOOR& tLastFloor) const
{
	_float fDistance;
	_vector vRayOrigin = vPos;
	vRayOrigin = XMVectorSetY(vRayOrigin, 100.f);
	const _vector vRayDir = _float4(0.f, -1.f, 0.f, 0.f);
	
	const _vector v0 = XMLoadFloat3(&tLastFloor.vRectPointPos[0]);
	const _vector v1 = XMLoadFloat3(&tLastFloor.vRectPointPos[1]);
	const _vector v2 = XMLoadFloat3(&tLastFloor.vRectPointPos[2]);
	
	if (TriangleTests::Intersects(vRayOrigin, vRayDir, v0, v1, v2, fDistance))
	{
		const _vector vFloorPos = vRayOrigin + vRayDir * fDistance;
		return XMVectorGetY(vFloorPos);
	}
	
	const _vector v3 = XMLoadFloat3(&tLastFloor.vRectPointPos[3]);
	if (TriangleTests::Intersects(vRayOrigin, vRayDir, v0, v2, v3, fDistance))
	{
		const _vector vFloorPos = vRayOrigin + vRayDir * fDistance;
		return XMVectorGetY(vFloorPos);
	}
	return -1.f;
}

CFloors* CFloors::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
 	CFloors*		pInstance = new CFloors(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFloors");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CFloors::Clone(void* pArg)
{
	CFloors*		pInstance = new CFloors(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CFloors");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFloors::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);

	m_Floors.clear();
}
