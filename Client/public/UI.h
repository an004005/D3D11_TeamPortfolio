#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Shader.h"

BEGIN(Engine)
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

enum class UI_PIVOT
{
	CENTER, LEFT_TOP, TOP, RIGHT_TOP, RIGHT, RIGHT_BOT, BOT, LEFT_BOT, LEFT, PIVOT_END
};

typedef struct tagImGuiUIInfo {

	_bool				bModify		= { false };
	_float2				fPosition = { 1.0f, 1.0f };
	_float2				fSize		= { 1.0f, 1.0f };

}IMGUIUIINFO;

class CUI abstract : public CGameObject
{
protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);

public:
	_uint Get_Priority() const {
		return m_iPriority;
	}
	void SetPriority(_uint iPriority) { 
		m_iPriority = iPriority;
	}

	_float2	Get_Position() const {
		return _float2{ m_fX, m_fY };
	}
	void Set_Position(_float2 vPos) {
		m_fX = vPos.x; 	m_fY = vPos.y;
	}

	_float2 Get_Size() const {
		return _float2{ m_fSizeX, m_fSizeY };
	}
	void Set_Size(_float2 vScale) {
		m_fSizeX = vScale.x; m_fSizeY = vScale.y;
	}

	_bool			IsCursorOn(POINT ptClient);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_double TimeDelta) override;
	virtual void	Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void	Imgui_RenderProperty() override;
	virtual void	SaveToJson(Json& json) override;
	virtual void	LoadFromJson(const Json& json) override;

protected:
	HRESULT			SetUp_Components();
	HRESULT			SetUp_ShaderResources();

	void			Imgui_UIInfo();

protected:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	ShaderParams		m_tParams;

protected:
	_float4x4			m_ViewMatrix;
	_float4x4			m_ProjMatrix;

	_float				m_fX = 0.5f;
	_float				m_fY = 0.5f;
	_float				m_fSizeX = 100.0f;
	_float				m_fSizeY = 100.0f;

	_uint				m_iPriority = 0;

	IMGUIUIINFO			m_ImguiInfo = {};


public:
	virtual CGameObject* Clone(void* pArg = nullptr) override = 0;
	virtual void Free() override;
};

END