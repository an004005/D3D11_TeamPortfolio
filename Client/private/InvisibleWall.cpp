#include "stdafx.h"
#include "InvisibleWall.h"
#include "GameInstance.h"
#include "VIBuffer_Trail_Instancing.h"
#include "MathUtils.h"
#include "GameUtils.h"
#include "ImguiUtils.h"
#include "MapInstance_Object.h"
#include "JsonStorage.h"
#include "JsonLib.h"
#include "PhysX_Manager.h"
#include "PhysXStaticModel.h"
#include "Level_Maptool.h"

CInvisibleWall::CInvisibleWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CInvisibleWall::CInvisibleWall(const CInvisibleWall& rhs)
    : CGameObject(rhs)
{
}

HRESULT CInvisibleWall::Initialize(void* pArg)
{
    CGameObject::Initialize(pArg);

    FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));
    FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Trail_VFX"), TEXT("Shader"), (CComponent**)&m_pShaderCom));

    m_pBuffer = CVIBuffer_Trail_Instancing::Create(m_pDevice, m_pContext, 1000);

    if (pArg)
    {
        Json& json = *static_cast<Json*>(pArg);

        LoadFromJson(json);
    }

    return S_OK;
}

void CInvisibleWall::Tick(_double TimeDelta)
{
    CGameObject::Tick(TimeDelta);
	
	RayPicking();
    
	m_pBuffer->Tick(TimeDelta);	// 
}

void CInvisibleWall::Late_Tick(_double TimeDelta)
{
    if (m_bVisible)
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CInvisibleWall::Render()
{
    FAILED_CHECK(SetUp_ShaderResources());

    return S_OK;
}

void CInvisibleWall::SaveToJson(Json& json)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	__super::SaveToJson(json);

	json["PointNumber"] = m_TrailPointList.size();

	for (_int i = 0; i < m_TrailPointList.size(); ++i)
	{

	}

    json["Width"] = m_fWidth;
    json["Pass"] = m_iPass;
}

void CInvisibleWall::LoadFromJson(const Json& json)
{
}

void CInvisibleWall::Imgui_RenderProperty()
{
	__super::Imgui_RenderProperty();
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_vPointPos = m_TrailPointList.back();
	ImGui::InputFloat3("PickingPtPos", &m_vPointPos.x); // 현재 피킹한 곳



}

HRESULT CInvisibleWall::SetUp_ShaderResources()
{
    return S_OK;
}

void CInvisibleWall::RayPicking()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	
	if (pGameInstance->KeyDown(DIKEYBOARD_P))
		m_bPick = true;

	if (pGameInstance->KeyDown(DIKEYBOARD_O))
		m_bPick = false;
	
	if (m_bPick == false) return;

	if (pGameInstance->KeyDown(CInput_Device::DIM_LB))
	{
		_float4 vOrigin;
		_float4 vDir;
		CGameUtils::GetPickingRay(vOrigin, vDir);

		physx::PxRaycastHit hitBuffer[1];
		physx::PxRaycastBuffer t(hitBuffer, 1);

		RayCastParams params;
		params.rayOut = &t;
		params.vOrigin = vOrigin;
		params.vDir = vDir;
		params.fDistance = 1000.f;
		params.iTargetType = CTB_STATIC;
		params.bSingle = true;

		if (pGameInstance->RayCast(params))
		{
			IM_LOG("Hit Ray");
			for (_uint i = 0; i < t.getNbAnyHits(); ++i)
			{
				auto p = t.getAnyHit(i);
				auto pComponent = CPhysXUtils::GetComponent(p.actor);


				auto iter = find_if(m_pMapObjects.begin(), m_pMapObjects.end(), [&](CMapObject* pMapObject) {

					//인스턴싱 물체면 안에 matrix를 추가로 탐색한다.
					if (pMapObject->Get_MapObjType() == INSTANCE)
					{
						auto com = dynamic_cast<CMapInstance_Object*>(pMapObject)->Find_PhysXStaticModel(dynamic_cast<CPhysXStaticModel*>(pComponent));
						return com == nullptr ? false : true;
					}
					else
					{
						return false;
					}

					});

				if (iter != m_pMapObjects.end())
				{
					m_pGameObject = *iter;		

					m_TrailPointList.push_back(m_pGameObject->GetTransform()->Get_State(CTransform::STATE_TRANSLATION));
				}
			}
		}
	}
}

void CInvisibleWall::ClearBuffer()
{
    m_pBuffer->ClearBuffer();
}

CInvisibleWall* CInvisibleWall::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInvisibleWall* pInstance = new CInvisibleWall(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CInvisibleWall");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CInvisibleWall::Clone(void* pArg)
{
    CInvisibleWall* pInstance = new CInvisibleWall(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CInvisibleWall");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CInvisibleWall::Free()
{
    CGameObject::Free();
    Safe_Release(m_pBuffer);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pRendererCom);
}
