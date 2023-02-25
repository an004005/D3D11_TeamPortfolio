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
// 		_float3 vScale; //로컬 크기값
// 		_float3 vRotation; //로컬 회전값
// 		_float3 vOffsetPos; //로컬 위치값
//
// 		_float fAppearTime; //나타나는 시간
// 		_float fDeleteTime; //사라지는 시간
//
// 		// // Mesh
// 		// _float fFadeInStartTime; //페이드 인 시작시간
// 		// _float fFadeInRatio; //페이드 인 비율
// 		// _float fFadeOutStartTime; //페이드 아웃 시작시간
// 		// _float fFadeOutRatio; //페이드 아웃 비율
// 		// // 
//
// 		// 단순 텍스쳐 (Point_Instance)
// 		_float fStartChangeTime; // Alpha Start
// 		_float fEndChangeTime;
// 		_float fChangeRatio; //변화 비율(시간에 따른 변화량)
// 		// ~단순 텍스쳐
//
// 		// For_UVSprite
// 		_bool bIsSprite; //스프라이트 텍스쳐 여부
// 		_bool bIsLoop; // 스프라이트 반복 여부
// 		_float2 vMaxUV; //최대 갯수(정수)
// 		_float fUVchangeTime; //스프라이트 변경 시간
// 		_float fUVchangeStartTime; //스프라이트 변경 시작 시간
// 		// ~For_UVSprite
//
// 		// For_Rotation
// 		_bool bIsBillBoard; //빌보드 여부
// 		_bool bIsRotation; //회전 여부
// 		_bool bIsParentRotation; //부모와 같이 회전하는지 여부
// 		_float3 vRotationToTime; //시간에 따른 회전
// 		// ~For_Rotation
//
// 		_float4 vColor; //이펙트 칼라
//
// 		_float2 vUVScale; //유브이 크기(곱해지는 양)
// 		_float2 vUVPos; //유브이 위치(더해지는 양)
//
// 		EffectType eType; //이펙트 종류
//
// 		_uint iTextureIndex; 
// 		_uint iPassIndex; //쉐이더 패스인덱스
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