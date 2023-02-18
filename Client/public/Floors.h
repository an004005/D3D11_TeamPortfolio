#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CGameInstance;
class CVIBuffer_PosRect;
END

BEGIN(Client)

typedef struct tagFloor
{
	_float4x4 WorldMatrix;
	_float3 vRectPointPos[4];
} FLOOR;

class CFloors : public CGameObject
{
private:
	CFloors(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFloors(const CFloors& rhs);
	virtual ~CFloors() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;
	virtual void SaveToJson(Json& json) override;
	void LoadJson(const Json& json);

public:
	_float GetHeight(_float4 vPos) const;

	void CreateFloor();
	void PickFloor();
	_bool PickFloorGetPos(OUT _float3& vPos);

private:
	void SetFloorPoint(FLOOR& tFloor);
	// yaw 회전이 없는 바닥을 가정하고 한다.
	_bool CheckOnFloor(const _float4& vPos, const FLOOR& tLastFloor) const;
	_float GetHeightOnFloor(const _float4& vPos, const FLOOR& tLastFloor) const;

private:
	static const array<_float3, 4> s_RectLocalPos;

	CGameInstance*			m_pGameInstance = nullptr;

	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_PosRect*		m_pVIBufferCom = nullptr;

	vector<FLOOR>			m_Floors;

	_uint m_iSelectedFloor = 0;

public:
	static CFloors* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};



END



