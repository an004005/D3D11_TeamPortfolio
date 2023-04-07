#pragma once
#include "ImguiObject.h"
#include "GameObject.h"
#include "Renderer.h"

BEGIN(Engine)

class ENGINE_DLL CModelTester final : public CGameObject
{
private:
	CModelTester(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CModelTester(const CModelTester& rhs);
	virtual ~CModelTester() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty();

public:
	void SetModel(const string & strModelTag);
	void SetRenderGroup(CRenderer::RENDERGROUP eGroup) { m_eRenderGroup = eGroup; }
	void SetPlayAnimation(const string& strAnim);
	void PlayAnimation(_float fRatio);

private:
	class CRenderer* m_pRendererCom = nullptr;
	class CModel* m_pModel = nullptr;
	CRenderer::RENDERGROUP m_eRenderGroup = CRenderer::RENDER_NONALPHABLEND_TOON;

public:
	static CModelTester* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

class ENGINE_DLL CImgui_CamAnimEditor : public CImguiObject
{
private:
	CImgui_CamAnimEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Imgui_RenderWindow() override;
	virtual void Imgui_RenderTab() override;

private:
	class CAnimCam* m_pAnimCam = nullptr;
	class CCamAnimation* m_pCurAnim = nullptr;

	unordered_map<string, CModelTester*> m_Models;
	unordered_map<string, list<pair<CModelTester*, string>>> m_AnimEvents;

public:
	static CImgui_CamAnimEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;
};

END