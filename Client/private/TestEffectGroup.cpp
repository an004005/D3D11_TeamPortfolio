#include "stdafx.h"
// #include "..\public\TestEffectGroup.h"
// #include "GameInstance.h"
// #include "EffectSystem.h"
// #include "JsonStorage.h"
// #include "CurveFloatMapImpl.h"
//
//
// CTestEffectGroup::CTestEffectGroup(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
// 	: CEffectGroup(pDevice, pContext)
// {
// }
//
// CTestEffectGroup::CTestEffectGroup(const CEffectGroup& rhs)
// 	: CEffectGroup(rhs)
// {
// }
//
// HRESULT CTestEffectGroup::Initialize(void* pArg)
// {
// 	const Json& json = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/Curve/ShockBurst.json");
// 	CEffectGroup::Initialize((void*)&json);
//
// 	{
// 		Json Sova_ShockCenter = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/EffectSystem/Rect/Sova_ShockCenter.json");
// 		m_pCenter = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, L"ProtoVFX_EffectSystem", &Sova_ShockCenter));
// 	}
// 	{
// 		Json Sova_ShockBurstIntro = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/EffectSystem/Rect/Sova_ShockBurstIntro.json");
// 		m_pIntro = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, L"ProtoVFX_EffectSystem", &Sova_ShockBurstIntro));
// 	}
// 	{
// 		Json Sova_ShockOutro = CJsonStorage::GetInstance()->FindOrLoadJson("../Bin/Resources/VFX/EffectSystem/Rect/Sova_ShockOutro.json");
// 		m_pOutro = dynamic_cast<CEffectSystem*>(CGameInstance::GetInstance()->Clone_GameObject_NoLayer(LEVEL_NOW, L"ProtoVFX_EffectSystem", &Sova_ShockOutro));
// 	}
//
// 	m_pIntro->GetTransform()->Set_Scaled(_float3{ 8.f, 8.f, 8.f });
// 	m_pOutro->GetTransform()->Set_Scaled(_float3{ 8.f, 8.f, 8.f });
//
// 	m_Timeline.SetTimelineLength(0.7);
// 	m_Timeline.SetFinishFunction(&m_Timeline, &CTimeline::Reset);
//
// 	if (m_Curves.empty())
// 	{
// 		AddEmptyCurve("CenterScale");
// 		AddEmptyCurve("IntroDissolve");
// 		AddEmptyCurve("OutroTime");
// 	}
//
// 	// 관리 주체가 m_Curves이기 때문에 addref한다.
// 	m_Timeline.SetCurve(this, &CTestEffectGroup::Tick_CenterScale, m_Curves.find("CenterScale")->second);
// 	Safe_AddRef(m_Curves.find("CenterScale")->second);
//
// 	m_Timeline.SetCurve(this, &CTestEffectGroup::Tick_IntroDissolve, m_Curves.find("IntroDissolve")->second);
// 	Safe_AddRef(m_Curves.find("IntroDissolve")->second);
//
// 	m_Timeline.SetCurve(this, &CTestEffectGroup::Tick_OutroTime, m_Curves.find("OutroTime")->second);
// 	Safe_AddRef(m_Curves.find("OutroTime")->second);
//
// 	m_Timeline.PlayFromStart();
//
// 	return S_OK;
// }
//
// void CTestEffectGroup::BeginTick()
// {
// 	CEffectGroup::BeginTick();
// }
//
// void CTestEffectGroup::Tick(_double TimeDelta)
// {
// 	CEffectGroup::Tick(TimeDelta);
// }
//
// void CTestEffectGroup::Late_Tick(_double TimeDelta)
// {
// 	__super::Late_Tick(TimeDelta);
// 	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
// }
//
// void CTestEffectGroup::Tick_CenterScale(_float fValue)
// {
// 	fValue *= 2.f;
// 	m_pCenter->GetTransform()->Set_Scaled(_float3(fValue, fValue, fValue));
// }
//
// void CTestEffectGroup::Tick_IntroDissolve(_float fValue)
// {
// 	m_pIntro->GetParams().Floats[0] = fValue;
//
// }
//
// void CTestEffectGroup::Tick_OutroTime(_float fValue)
// {
// 	m_pOutro->GetParams().Floats[0] = fValue;
// }
//
// HRESULT CTestEffectGroup::Render()
// {
// 	m_pIntro->GetTransform()->CopyState(CTransform::STATE_TRANSLATION, m_pTransformCom);
// 	m_pOutro->GetTransform()->CopyState(CTransform::STATE_TRANSLATION, m_pTransformCom);
// 	m_pCenter->GetTransform()->CopyState(CTransform::STATE_TRANSLATION, m_pTransformCom);
//
// 	m_pIntro->Render();
// 	m_pOutro->Render();
// 	m_pCenter->Render();
// 	return S_OK;
// }
//
//
//
// void CTestEffectGroup::Imgui_RenderProperty()
// {
// 	CEffectGroup::Imgui_RenderProperty();
// }
//
// CTestEffectGroup* CTestEffectGroup::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
// {
// 	CTestEffectGroup*		pInstance = new CTestEffectGroup(pDevice, pContext);
//
// 	if (FAILED(pInstance->Initialize_Prototype()))
// 	{
// 		MSG_BOX("Failed to Created : CTestEffectGroup");
// 		Safe_Release(pInstance);
// 	}
// 	return pInstance;
// }
//
// CGameObject* CTestEffectGroup::Clone(void* pArg)
// {
// 	CTestEffectGroup*		pInstance = new CTestEffectGroup(*this);
//
// 	if (FAILED(pInstance->Initialize(pArg)))
// 	{
// 		MSG_BOX("Failed to Cloned : CTestEffectGroup");
// 		Safe_Release(pInstance);
// 	}
// 	return pInstance;
// }
//
// void CTestEffectGroup::Free()
// {
// 	CEffectGroup::Free();
// 	Safe_Release(m_pCenter);
// 	Safe_Release(m_pIntro);
// 	Safe_Release(m_pOutro);
// }
