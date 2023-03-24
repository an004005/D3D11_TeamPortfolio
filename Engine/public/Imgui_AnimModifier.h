#pragma once
#include "ImguiObject.h"
#include "GameObject.h"
#include "Renderer.h"

BEGIN(Engine)

class CBaseAnimInstance;

typedef struct tagCamKeyFrame
{
	string		m_szAnimName;
	_float4		m_vKeyFramePos;
	_float		m_fKeyFrameRatio;

}CAMKEYFRAME;

/************************
 * CModelPreviwer
 ************************/
class ENGINE_DLL CModelPreviwer final : public CGameObject
{
private:
	CModelPreviwer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModelPreviwer(const CModelPreviwer& rhs);
	virtual ~CModelPreviwer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	void SetAttachTo(string BoneName, CModelPreviwer* pAttachPreview);

	virtual void Imgui_RenderProperty();

public:
	class CAnimation* GetPlayAnimation();
	class CModel* GetModel() { return m_pModel; }
	void Reset();

	void SetRenderGroup(CRenderer::RENDERGROUP eRenderGroup) { m_eRenderGroup = eRenderGroup; }

private:
	class CRenderer*				m_pRendererCom = nullptr;
	class CModel*					m_pModel = nullptr;
	wstring m_ModelName;

	string m_AttachBoneName;
	CModelPreviwer* m_pAttachPreview = nullptr;

	_bool	m_bLocalMoveAccess = false;

	Vector3	m_vOptionalVector = { 0.f, 0.f, 0.f };

	CRenderer::RENDERGROUP m_eRenderGroup = CRenderer::RENDER_NONALPHABLEND_TOON;

public:
	static CModelPreviwer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

/************************
 * CImgui_AnimModifier
 ************************/
class ENGINE_DLL CImgui_AnimModifier final : public CImguiObject
{
private:
	CImgui_AnimModifier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Imgui_RenderTab() override;
	static CImgui_AnimModifier* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;

private:
	void RootMotionMaker(); // 루트모션메이커

private:
	void ActionCamMaker();	// 액션캠메이커

	string				m_szAnimName = "";
	_float4				m_vOrigin = _float4(0.f, 0.f, 0.f, 1.f);
	_float				m_fRatio = 0.f;
	list<CAMKEYFRAME>	m_CamFrameList;


private:
	class CGameObject* m_pSelectedObject = nullptr;
	class CGameInstance* m_pGameInstance = nullptr;


	CModelPreviwer* m_pPreview = nullptr;

private:

};

END