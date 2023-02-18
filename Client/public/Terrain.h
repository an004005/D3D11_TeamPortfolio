#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Terrain;
class CGameInstance;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
public:
	struct TerrainDesc
	{
		const char* pFilterPath = nullptr;
		const char* pBlendTexPaths[4]{};
	};
	enum TEXTURE { TYPE_DIFFUSE, TYPE_BRUSH, TYPE_END };
private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

	void Ready_Filter();
	bool PickTerrain(OUT _float4& vPickPos);

	void SetEditMode(_bool bEditMode) { m_bEditMode = bEditMode ? 1 : 0; }
	void SetBrushRange(_float fRange) { m_fBrushRange = fRange; }
	void SetBrushPos(_float4 vPos) { m_vBrushPos = vPos; }

	void OverwriteFilterTex();
	void EditFilter(_float fPower, _uint m_iEditFilter);
	void SaveFilter();
	void ResetFilter();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom[TYPE_END] = { nullptr };
	CVIBuffer_Terrain*		m_pVIBufferCom = nullptr;


	vector<PIXEL> m_pFilterPixels;
	_uint m_iFilterWidth;
	_uint m_iFilterHeight;
	ID3D11ShaderResourceView* m_pFilterSRV = nullptr;

	_float4 m_vBrushPos;
	_float m_fBrushRange = 5.f;
	_uint m_bEditMode = 0;

	CGameInstance* m_pGameInstance = nullptr;


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END