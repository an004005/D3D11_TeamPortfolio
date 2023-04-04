#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CRigidBody;
END


BEGIN(Client)

class CRedString : public CGameObject
{
public:
	CRedString(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRedString(const CRedString& rhs);
	virtual ~CRedString() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

	virtual void LoadFromJson(const Json& json) override;
	virtual void SaveToJson(Json& json) override;

public:
	void CreateString();

private:
	void DeletePoint(_uint idx);
	_bool PickPoint();
	void AddPoint_ForTool(_float4 vPos);

private:
	CShader*							m_pShaderCom = nullptr;
	CRenderer*							m_pRendererCom = nullptr;
	class CVIBuffer_Line_Instancing*		m_pBuffer = nullptr;

	vector<pair<_float4, CRigidBody*>> m_Points;
	_int m_iPointIdx = 0;

	_float m_fRadius = 0.1f;
	_float m_fCurveSmooth = 50.f; // ���� ���� �ε巴�Եǰ�, ����Ʈ ������ ������
	_float m_fIndicatorSize = 0.2f;

	_bool m_bOnTool = false;
	_float m_fCreateCoolTime = 0.5f;
	_float m_fMaxCreateCoolTime = 0.5f;



public:
	static CRedString*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

END