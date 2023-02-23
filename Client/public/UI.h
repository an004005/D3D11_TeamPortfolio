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

// 이 UI가 포함되어 있는 캔버스(또는 뷰포트)의 스크린 스페이스의 RECT사이즈
struct CanvasRect
{
	_float fCenterX = 0.f;
	_float fCenterY = 0.f;
	_float fHalfWidth = 0.f;
	_float fHalfHeight = 0.f;

	CanvasRect() = default;
	CanvasRect(_float fCenterX, _float fCenterY, _float fHalfWidth, _float fHalfHeight)
		: fCenterX(fCenterX),
		fCenterY(fCenterY),
		fHalfWidth(fHalfWidth),
		fHalfHeight(fHalfHeight) {}
};

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

	void			Set_CanvasSize(CanvasRect CanvasSize) { m_CanvasSize = CanvasSize; }
	_float2			Get_PivotXY(UI_PIVOT ePivot) const;

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

protected:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	ShaderParams		m_tParams;

protected:
	_float4x4			m_ViewMatrix;
	_float4x4			m_ProjMatrix;

	CanvasRect			m_CanvasSize;

	_float				m_fX = 0.0f;
	_float				m_fY = 0.0f;
	_float				m_fSizeX = 0.0f;
	_float				m_fSizeY = 0.0f;

	_uint				m_iPriority = 0;

	_float2				m_vScale = _float2::One;
	UI_PIVOT			m_ePivot = UI_PIVOT::CENTER;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) override = 0;
	virtual void Free() override;
};

END