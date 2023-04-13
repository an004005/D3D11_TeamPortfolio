#pragma once
#include "MapObject.h"

BEGIN(Engine)
class CPhysXStaticModel;
END

BEGIN(Client)

class CMapInstance_Object : public CMapObject
{
private:
	CMapInstance_Object(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMapInstance_Object(const CMapInstance_Object& rhs);
	virtual ~CMapInstance_Object() = default;

public:
	HRESULT Initialize_Prototype() override;
	HRESULT Initialize(void* pArg) override;
	void BeginTick() override;
	void Tick(_double TimeDelta) override;
	void Late_Tick(_double TimeDelta) override;
	HRESULT Render() override;
	virtual HRESULT Render_ShadowDepth() override;
	virtual void LoadFromJson(const Json& json) override;
	virtual void SaveToJson(Json& json) override;
	virtual void Imgui_RenderProperty() override;

public:
	CModel_Instancing* Get_Model_Instancing() { return m_pModel_InstancingCom; }
	const CPhysXStaticModel* Find_PhysXStaticModel(CPhysXStaticModel* pPhsXSM);
	_float4	Get_FocusPosition();
public:
	//툴에서 생성이나 삭제 후 자동으로 다음이나 이전 객체를 선택
	void		Set_Focus();
	void		Create_PhsyX(_fmatrix WorldMatrix);

	static _bool s_bPhysX;
private:
	wstring MakePxModelProtoTag();
private:
	HRESULT	SetUp_Components();

	vector<CPhysXStaticModel*>		m_pPxModels;
	CModel_Instancing*				m_pModel_InstancingCom = nullptr;

	wstring							m_PxModelTag;

	_int					m_iIndex = -1;
	_float4x4				m_WorldMatrix;

	_uint					m_iComNum = 0;

public:
	static CMapInstance_Object* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg = nullptr) override;
	void Free() override;
};

END