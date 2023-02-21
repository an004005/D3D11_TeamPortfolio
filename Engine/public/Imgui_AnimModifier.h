#pragma once
#include "ImguiObject.h"
#include "GameObject.h"

BEGIN(Engine)

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

public:
	class CAnimation* GetPlayAnimation();
	class CModel* GetModel() { return m_pModel; }
	void Reset();


private:
	class CRenderer*				m_pRendererCom = nullptr;
	class CModel*					m_pModel = nullptr;
	wstring m_ModelName;

	string m_AttachBoneName;
	CModelPreviwer* m_pAttachPreview = nullptr;
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
	class CGameObject* m_pSelectedObject = nullptr;
	class CGameInstance* m_pGameInstance = nullptr;


	CModelPreviwer* m_pPreview = nullptr;

private:

};

END