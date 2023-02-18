#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CMaterialPreview : public CGameObject
{
private:
	CMaterialPreview(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMaterialPreview(const CMaterialPreview& rhs);
	virtual ~CMaterialPreview() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

private:
	class CMaterial* m_pMaterial = nullptr;
	class CRenderer* m_pRendererCom = nullptr;
	class CVIBuffer* m_pBuffer = nullptr;
	class CModel* m_pModel = nullptr;
public:
	static CMaterialPreview* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

};

END