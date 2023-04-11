#include "stdafx.h"
#include "Sheath_Player.h"
#include "Bone.h"
#include "PlayerInfoManager.h"

CSheath_Player::CSheath_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CGameObject(pDevice, pContext)
{
}

CSheath_Player::CSheath_Player(const CSheath_Player& rhs)
	: CGameObject(rhs)
{
}

HRESULT CSheath_Player::Initialize_Prototype()
{
	FAILED_CHECK(__super::Initialize_Prototype());

	return S_OK;
}

HRESULT CSheath_Player::Initialize(void* pArg)
{
	FAILED_CHECK(CGameObject::Initialize(pArg));

	FAILED_CHECK(SetUp_Components());

	m_pModel = m_pModelComs.front();
	m_eType = WP_0106;

	return S_OK;
}

void CSheath_Player::BeginTick()
{
}

void CSheath_Player::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CSheath_Player::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	m_pModel->Tick(TimeDelta);
	if (nullptr != m_pRenderer && m_bVisible)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND_TOON, this);
}

void CSheath_Player::AfterPhysX()
{
}

HRESULT CSheath_Player::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pModel->Render(m_pTransformCom);

	return S_OK;
}

void CSheath_Player::Setup_BoneMatrix(CModel* pModel, _fmatrix Transform)
{
	_matrix	SocketMatrix = pModel->GetBoneMatrix("Sheath") * Transform;

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	m_pTransformCom->Set_WorldMatrix(SocketMatrix);
}

void CSheath_Player::Change_Sheath(WEAPONTYPE eType)
{
	if (m_eType == eType) return;
	Assert(eType < WP_END);

	m_eType = eType;
	CPlayerInfoManager::GetInstance()->Set_PlayerWeapon(static_cast<_uint>(m_eType));

	m_pModel = m_pModelComs[eType];
}

HRESULT CSheath_Player::SetUp_Components()
{
	FAILED_CHECK(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRenderer));

	CModel* pModel = nullptr;

	// wp0107
	m_pModelTags.push_back(L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_107/wp0107.static_model");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW,
		L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_107/wp0107.static_model",
		L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_107/wp0107.static_model",
		(CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	// wp0127
	m_pModelTags.push_back(L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_127/wp0127.static_model");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW,
		L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_127/wp0127.static_model",
		L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_127/wp0127.static_model",
		(CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	// wp0191
	m_pModelTags.push_back(L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_191/wp0191.static_model");
	FAILED_CHECK(__super::Add_Component(LEVEL_NOW,
		L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_191/wp0191.static_model",
		L"../Bin/Resources/Meshes/Scarlet_Nexus/StaticModel/wp_191/wp0191.static_model",
		(CComponent**)&pModel));
	m_pModelComs.push_back(pModel);

	return S_OK;
}

CSheath_Player* CSheath_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSheath_Player* pInstance = new CSheath_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSheath_Player");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CSheath_Player::Clone(void* pArg)
{
	CSheath_Player* pInstance = new CSheath_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSheath_Player");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSheath_Player::Free()
{
	__super::Free();

	m_pModel = nullptr;
	for (auto& iter : m_pModelComs)
	{
		Safe_Release(iter);
	}

	Safe_Release(m_pRenderer);
	//Safe_Release(m_pModel);
}
