#pragma once
#include "GameObject.h"
#include "Client_Defines.h"
#include "SAS_Cable.h"

BEGIN(Engine)
class CModel;
class CPxModel;
class CAnimation;
class CFSMComponent;
END

BEGIN(Client)



class CBrainField : public CGameObject
{
};


/**********************
 * CBrainFieldCables
 **********************/
class CBrainFieldCables : public CGameObject
{
private:
	enum EBrainFieldCable
	{
		HEAD_1, // �Ӹ� ������
		HEAD_2, // �Ӹ� ������ ��
		HEAD_3, // �Ӹ� �ƶ�
		BACK_1, // �� ���� ��
		BACK_2, // �� ������ ��
		BACK_3, // �� ���
		BACK_4, // �� ���� �Ʒ�
		BACK_5, // �� ������ �Ʒ�
		CABLE_END
	};

private:
	CBrainFieldCables(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBrainFieldCables(const CBrainFieldCables& rhs);
	virtual ~CBrainFieldCables() = default;

public:
	HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Imgui_RenderProperty() override;

public:
	void Activate(_bool bActive); // ���� �� �ٷ� ����

public:
	void SetTargetInfo(CTransform* pTargetTransform, CModel* pTargetModel);

private:
	void SetUpFSM();

private:
	array<CPxModel*, CABLE_END> m_CableModels{};
	array<_float4x4, CABLE_END> m_CablePivots{};
	array<string, CABLE_END> m_CableBones{};

	CFSMComponent* m_pFSM = nullptr;

	_bool m_bActive = false;

	EBrainFieldCable m_eRenderEndLine = CABLE_END;

	CTransform* m_pTargetTransform = nullptr;
	CModel*		m_pTargetModel = nullptr;
	CRenderer* m_pRendererCom = nullptr;

public:
	virtual void Free() override;
};

END