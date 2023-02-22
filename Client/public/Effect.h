#pragma once
// #include "Client_Defines.h"
// #include "GameObject.h"
//
// BEGIN(Engine)
// class CRenderer;
// END
//
// BEGIN(Client)
// class CEffect abstract : public CGameObject
// {
// public:
// 	enum EffectType { EFFECT_PARTICLE_POINT, EFFECT_PARTICLE_RECT, EFFECT_PARTICLE_MESH, EFFECT_RECT, EFFECT_MESH, EFFECT_END };
//
// public:
// 	typedef struct EffectInfo
// 	{
// 		wstring pEffectTag= L"";
// 		wstring pMeshPrototypeTag = L""; // For Mesh
//
// 		_float3 vScale; //���� ũ�Ⱚ
// 		_float3 vRotation; //���� ȸ����
// 		_float3 vOffsetPos; //���� ��ġ��
//
// 		_float fAppearTime; //��Ÿ���� �ð�
// 		_float fDeleteTime; //������� �ð�
//
// 		// // Mesh
// 		// _float fFadeInStartTime; //���̵� �� ���۽ð�
// 		// _float fFadeInRatio; //���̵� �� ����
// 		// _float fFadeOutStartTime; //���̵� �ƿ� ���۽ð�
// 		// _float fFadeOutRatio; //���̵� �ƿ� ����
// 		// // 
//
// 		// �ܼ� �ؽ��� (Point_Instance)
// 		_float fStartChangeTime; // Alpha Start
// 		_float fEndChangeTime;
// 		_float fChangeRatio; //��ȭ ����(�ð��� ���� ��ȭ��)
// 		// ~�ܼ� �ؽ���
//
// 		// For_UVSprite
// 		_bool bIsSprite; //��������Ʈ �ؽ��� ����
// 		_bool bIsLoop; // ��������Ʈ �ݺ� ����
// 		_float2 vMaxUV; //�ִ� ����(����)
// 		_float fUVchangeTime; //��������Ʈ ���� �ð�
// 		_float fUVchangeStartTime; //��������Ʈ ���� ���� �ð�
// 		// ~For_UVSprite
//
// 		// For_Rotation
// 		_bool bIsBillBoard; //������ ����
// 		_bool bIsRotation; //ȸ�� ����
// 		_bool bIsParentRotation; //�θ�� ���� ȸ���ϴ��� ����
// 		_float3 vRotationToTime; //�ð��� ���� ȸ��
// 		// ~For_Rotation
//
// 		_float4 vColor; //����Ʈ Į��
//
// 		_float2 vUVScale; //������ ũ��(�������� ��)
// 		_float2 vUVPos; //������ ��ġ(�������� ��)
//
// 		EffectType eType; //����Ʈ ����
//
// 		_uint iTextureIndex; 
// 		_uint iPassIndex; //���̴� �н��ε���
//
// 		_bool bIsBlur;
// 		_bool bIsBloom;
// 		_bool bIsGlow;
//
// 		// PARTICLEDESC tParticleDesc;
// 	}EFFECTINFO;
//
// public:
// 	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
// 	CEffect(const CEffect& rhs);
// 	virtual ~CEffect() = default;
//
// public:
// 	virtual HRESULT			Initialize_Prototype() override;
// 	virtual HRESULT			Initialize(void* pArg) override;
//
// 	virtual void			BeginTick() override;
// 	virtual void			Tick(_double TimeDelta) override;
// 	virtual void			Late_Tick(_double TimeDelta) override;
// 	virtual HRESULT			Render() override;
//
// public:
// 	EFFECTINFO				Get_EffectInfo() { return m_tEffect; }
//
// public:
// 	virtual CEffect*		Start(CTransform* vTransform);
// 	virtual void			End();
// 	void					ResetAlphaRatio() { m_fAlphaRatio = 0.f; }
//
// protected:
// 	CRenderer*				m_pRendererCom = nullptr;
// 	CTransform*				m_pOwnerTransform = nullptr;
//
// 	EFFECTINFO				m_tEffect;
//
// 	_float4x4				m_LocalMatrix;
// 	_float4x4				m_WorldMatrix;
// 	_float4x4				m_WorldMatrixTP;
//
// 	_float					m_fTimeAcc = 0.f;
// 	_float					m_fRatio = 0.f;
//
// 	_float					m_fAlphaRatio = 0.f;
//
// 	_float2					m_vSpriteUVPos;
//
// protected:
// 		virtual HRESULT		SetUp_Components();
//
// public:
// 	virtual CGameObject*	Clone(void* pArg) = 0;
// 	virtual void			Free() override;
// };
//
// END