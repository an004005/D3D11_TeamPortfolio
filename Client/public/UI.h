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
	CENTER,		//0
	LEFT_TOP,	//1
	TOP,		//2
	RIGHT_TOP,	//3 
	RIGHT,		//4
	RIGHT_BOT,	//5
	BOT,		//6
	LEFT_BOT,	// 7
	LEFT,		// 8
	PIVOT_END	// 9
};

enum
{
	UI_PIVOT_CNT = static_cast<_uint>(UI_PIVOT::PIVOT_END)
};

// 이 UI가 포함되어 있는 캔버스(또는 뷰포트)의 스크린 스페이스의 RECT사이즈
struct CanvasRect
{
	_float fCenterX = { 0.0f };
	_float fCenterY = { 0.0f };
	_float fHalfWidth = { 0.0f };
	_float fHalfHeight = { 0.0f };

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
	_bool	Get_TempOff() { return m_bTempOff; }
	void	TempOff(const _bool bOff);

	_float2	GetScreenSpaceLeftTop();
	void SetCanvasSize(CanvasRect CanvasSize) { m_CanvasSize = CanvasSize; }
	
	//UI_PIVOT GetPivot() const { 
	//	return m_ePivot;
	//}
	void SetPivot(UI_PIVOT ePivot) { 
		m_ePivot = ePivot;
	}

	//_uint Get_Priority() const {
	//	return m_iPriority;
	//}
	//void Set_Priority(_uint iPriority) { 
	//	m_iPriority = iPriority;
	//}

	_float2 GetScale() const { 
		return m_vScale;
	}
	void SetScale(_float2 vScale) {
		m_vScale = vScale;
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

	void SetUseRotation(_bool bUseRot);

protected:
	_float2			GetPivotXY(UI_PIVOT ePivot) const;

protected:
	CShader*			m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	ShaderParams		m_tParams;

protected:
//	_uint				m_iPriority = 0;			// 낮을 수록 뒤에 그려진다.

	_float				m_fX = 0.0f;
	_float				m_fY = 0.0f;
	_float				m_fSizeX = 100.0f;
	_float				m_fSizeY = 100.0f;
	_float2				m_vScale = { 1.0f, 1.0f };	// 추가적으로 수정할 스케일

	_bool m_bUseRot = false;
	// pivot중심의 공전 각도
	_float m_fRadianRevolve = 0.f;
	// 현재 위치 기준 자전 각도
	_float m_fRadianRotation = 0.f;

	CanvasRect			m_CanvasSize;
	UI_PIVOT			m_ePivot = UI_PIVOT::CENTER;
	
	_bool				m_bTempOff = { false };

public:
	virtual CGameObject* Clone(void* pArg = nullptr) override = 0;
	virtual void Free() override;
};

END