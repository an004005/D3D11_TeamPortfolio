#pragma once
#include "GameObject.h"

BEGIN(Engine)

enum class UI_PIVOT
{
	CENTER, LEFT_TOP, TOP, RIGHT_TOP, RIGHT, RIGHT_BOT, BOT, LEFT_BOT, LEFT, PIVOT_END
};

enum
{
	UI_PIVOT_CNT = static_cast<_uint>(UI_PIVOT::PIVOT_END)
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
		  fHalfHeight(fHalfHeight){}
};

class ENGINE_DLL CUI abstract : public CGameObject
{
protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;

	virtual void SaveToJson(Json& json) override;

	virtual void Imgui_RenderComponentProperties() override;
	virtual void Imgui_RenderProperty() override;
	

public:
	void SetCanvasSize(CanvasRect CanvasSize) { m_CanvasSize = CanvasSize; }
	void SetUseRotation(_bool bUseRot);
	void SetPriority(_uint iPriority) { m_iPriority = iPriority;}
	void SetPosition(_float2 vPos) { m_fX = vPos.x; m_fY = vPos.y; }
	void SetSize(_float2 vScale) { m_fSizeX = vScale.x; m_fSizeY = vScale.y; }
	void SetPivot(UI_PIVOT ePivot) { m_ePivot = ePivot; }
	void SetScale(_float2 vScale) { m_vScale = vScale; }
	void SetRotation(_float fRadian) { m_fRadianRotation = fRadian; }
	void SetRevolve(_float fRadian) { m_fRadianRevolve = fRadian; }

public:
	_bool IsCursorOn(POINT ptClient);
	_bool IsCursorOn();
	_uint GetPriority() const { return m_iPriority; }
	_float2 GetScreenSpaceLeftTop();
	_float2 GetPosition() const { return _float2{ m_fX, m_fY }; }
	_float2 GetSize() const { return _float2{ m_fSizeX, m_fSizeY }; }
	UI_PIVOT GetPivot() const { return m_ePivot; }
	_float2 GetScale() const { return m_vScale; }
	_float GetRotation() const { return m_fRadianRotation; }
	_float GetRevolve() const { return m_fRadianRevolve; }

public:
	void RenderFont(const _tchar * pFontTag, const _tchar * pText, _float2 vScale, _fvector vColor, _uint iFontSize);

protected:
	_float2 GetPivotXY(UI_PIVOT ePivot) const;

protected:
	class CRenderer*				m_pRendererCom = nullptr;


	CanvasRect m_CanvasSize;

	// fx, fy는 pivot과 CanvasSize 기준의 상대 위치 입니다.
	_float m_fX = 0.f;
	_float m_fY = 0.f;
	_float m_fSizeX = 0.f;
	_float m_fSizeY = 0.f;
	_float2 m_vScale = _float2::One;
	UI_PIVOT m_ePivot = UI_PIVOT::CENTER;

	_bool m_bUseRot = false;
	// pivot중심의 공전 각도
	_float m_fRadianRevolve = 0.f;
	// 현재 위치 기준 자전 각도
	_float m_fRadianRotation = 0.f;

	// 낮은 UI가 먼저 그려진다.(덮어 씌워진다.)
	_uint m_iPriority = 0;

public:
	virtual void Free() override;
};

END