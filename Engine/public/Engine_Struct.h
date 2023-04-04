#pragma once
namespace Engine
{
	typedef struct tagGraphicDesc
	{
		enum WINMODE { WINMODE_FULL, WINMODE_WIN, WINMODE_END };

		WINMODE			eWindowMode;
		unsigned int	iViewportSizeX;
		unsigned int	iViewportSizeY;
		HWND			hWnd;		
	}GRAPHIC_DESC;

	typedef struct tagPixel_BGRA
	{
		union
		{
			struct
			{
				unsigned char b;
				unsigned char g;
				unsigned char r;
				unsigned char a;
			};
			unsigned char m[4];
			unsigned long color;
		};
		tagPixel_BGRA() : color(0){}
		tagPixel_BGRA(unsigned long color) : color(color) {}
		tagPixel_BGRA(unsigned char b, unsigned char g, unsigned char r, unsigned char a) : b(b), g(g), r(r), a(a) {}

	}PIXEL;

	typedef struct tagDecomposedMatrix
	{
		_float3	 vScale;
		Quaternion vRotation;
		_float3 vPosition;
	}DECOMPOSED_MATRIX;

	// typedef struct tagModelMaterial
	// {
	// 	class CTexture*		pTexture[AI_TEXTURE_TYPE_MAX];
	// 	
	// }MODELMATERIAL;

	/* 방향성광원, 점광원 : 방향벡터가 필요하다. */
	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_FOV, TYPE_CAPSULE, TYPE_END };

		TYPE				eType;

		bool				isEnable;

		_float4			vDirection;

		/* 모든 정점(픽셀)이 받는 빛의 방향벡터가 다 다르게 표현되어야하기때문이다. */
		/* 셰)이더내에서 빛의 방향벡터를 구해서 연산한다. */
		_float4			vPosition;
		float				fRange;

		_float4				vCapsuleEnd;
		_float4				vCapsuleStart;

		_float4			vDiffuse;
		_float4			vAmbient;
		_float4			vSpecular;
	}LIGHTDESC;	

	typedef struct tagKeyframe
	{
		double		Time;
		XMFLOAT3	vScale;
		XMFLOAT4	vRotation;
		XMFLOAT3	vPosition;
	}KEYFRAME;

	typedef struct tagFaceIndices16
	{
		unsigned short			_0, _1, _2;
	}FACEINDICES16;

	typedef struct tagFaceIndices32
	{
		unsigned long			_0, _1, _2;
	}FACEINDICES32;

	typedef struct tagVertexLineColor
	{
		_float3		vPosition;
		_float4		vColor;
	}VTXLINE;

	typedef struct ENGINE_DLL tagVertexLineColor_Declaration
	{
		static const unsigned int		iNumElements = 2;
		static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];			
	}VTXLINE_DECLARATION;


	typedef struct tagVertexPosition
	{
		XMFLOAT3		vPosition;
	}VTXPOS;


	typedef struct ENGINE_DLL tagVertexPosition_Declaration
	{
		static const unsigned int		iNumElements = 1;
		static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];			
	}VTXPOS_DECLARATION;

	typedef struct tagVertexPositionTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexUV;
	}VTXTEX;

	typedef struct ENGINE_DLL tagVertexPositionTexture_Declaration
	{
		static const unsigned int		iNumElements = 2;
		static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];			
	}VTXTEX_DECLARATION;

	typedef struct tagVertexPositionNormalTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
	}VTXNORTEX;

	typedef struct ENGINE_DLL tagVertexPositionNormalTexture_Declaration
	{
		static const unsigned int		iNumElements = 3;
		static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXNORTEX_DECLARATION;


	typedef struct tagVertexModel
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;		
	}VTXMODEL;

	typedef struct ENGINE_DLL tagVertexModel_Declaration
	{
		static const unsigned int		iNumElements = 4;
		static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXMODEL_DECLARATION;

	typedef struct tagVertexAnimModel
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndex; /* 이 정점이 사용해야하는 뼈의 인덱스(전체뼈 중의 순서(x), 이 정점이 소속된 메시에 영향을 주는 뼈중의 순서(o))  */
		XMFLOAT4		vBlendWeight;
	}VTXANIMMODEL;

	typedef struct ENGINE_DLL tagVertexAnimModel_Declaration
	{
		static const unsigned int		iNumElements = 6;
		static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXANIMMODEL_DECLARATION;



	typedef struct tagValorantBaseVertexModel
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
	}VAL_BASE_VTXMODEL;

	typedef struct ENGINE_DLL tagValorantBaseVertexModel_Declaration
	{
		static const unsigned int		iNumElements = 4;
		static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VAL_BASE_VTXMODEL_DECLARATION;


	// For StaticModel Instancing
	typedef struct tagVertexStaticModelMatrix
	{
		_float4			vRight;
		_float4			vUp;
		_float4			vLook;
		_float4			vPosition;
	}VTXMODELMATRIX;

	// For Particle PointInstance 
	typedef struct tagVertexMatrix
	{
		_float4			vRight;
		_float4			vUp;
		_float4			vLook;
		_float4			vPosition;

		_float4			vRotRight;
		_float4			vRotUp;
		_float4			vRotLook;
		_float4			vRotPos;

		_float			fGravityPower;
		// _bool			bOnSurfase;

		_uint		NearestIndex;
	}VTXMATRIX;

	typedef struct ENGINE_DLL tagVertexPoint_Instance_Particle_Declaration
	{
		static const unsigned int		iNumElements = 10;
		static D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOINT_INSTANCE_PARTICLE_DECLARATION;

	// ~ For Particle PointInstance 

	typedef struct tagVertex_Instance_DrawData
	{
		_float4			vRight;
		_float4			vUp;
		_float4			vLook;
		_float4			vPosition;
		_float4			vColor;
	}VTXINSTANCE_DRAWDATA;

	typedef struct tagVertex_Instance
	{
		_float4			vRight;
		_float4			vUp;
		_float4			vLook;
		_float4			vPosition;
		_float4			vColor;

		_float3			vEulerRad;
		_float3			vRandDir;
		_float3			vVelocity;
		_float3			vSize;
		_float			fGravityPower;
		_float			fLifeTime;
		_float			fCurLifeTime;
		class CRigidBody* pRigidBody = nullptr;
	}VTXINSTANCE;

	/*******************
	VTXMODEL INSTANCING
	********************/
	typedef struct ENGINE_DLL tagVertexInstance_Declaration
	{
		static const unsigned int		iNumElements = 8;
		static D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXMODEL_INSTANCE_DECLARATION;


	typedef struct tagVertexPoint
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vPSize;
	}VTXPOINT;

	typedef struct ENGINE_DLL tagVertexPoint_Declaration
	{
		static const unsigned int		iNumElements = 6;
		static D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXPOINT_DECLARATION;

	typedef struct ENGINE_DLL tagVertex_Mesh_Instance_Declaration
	{
		static const unsigned int		iNumElements = 9;
		static D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	} VTXMESH_INSTANCE_DECLARATION;

	typedef struct tagVertexPositionCubeTexture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexUV;
	}VTXCUBETEX;

	typedef struct ENGINE_DLL tagVertexPositionCubeTexture_Declaration
	{
		static const unsigned int			iNumElements = 2;
		static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];
	}VTXCUBETEX_DECLARATION;

	// typedef struct EffectParticleDesc
	// {
	// 	_float3 vMinPos; //최소 위치
	// 	_float3 vMaxPos; //최대 위치
	//
	// 	_float fMaxSpeed; //최대 속력
	// 	_float fMinSpeed; //최소 속력
	// 	_float3 vMaxDir; //방향(노말라이즈)
	// 	_float3 vMinDir; //방향(노말라이즈)
	// 	_float3 vAcceleration; //속도 변화량
	//
	// 	_float3 vMinScale; //최소 크기
	// 	_float3 vMaxScale; //최대 크기
	// 	_float3 vScaleVariation; //크기 변화량
	//
	// 	_float4 vMinColor; //최소 칼라
	// 	_float4 vMaxColor; //최대 칼라
	// 	_float4 vColorVariation; //색 변화량
	//
	// 	_float fMinLifeTime; //최소 생존시간
	// 	_float fMaxLifeTime; //최대 생존시간
	//
	// 	_float fGenerateRate; //파티클 생성 비율(초당 파티클 수)
	// 	_float fGenerationTime; //파티클 생성 시간(몇초까지 생성할 것인지)
	// 	_uint  iTotalCnt;//전체 파티클 갯수
	//
	// }PARTICLEDESC;

	// typedef struct Particle
	// {
	// 	_float3 vPos;
	// 	_float4	vColor;
	// 	_float3	vSize;
	// 	_float3 vVelocity;
	// 	_float fLifeTime;
	// 	_float fAge; //현재 파티클 생존시간
	// 	_uint iNum; //인스탄스 번호
	// }PARTICLE;

	struct PxMesh
	{
		physx::PxTriangleMesh* pPxMesh = nullptr;
		physx::PxTriangleMeshGeometry tMeshGeom;
		physx::PxShape* pShape = nullptr;
	};


	typedef struct tagVertexLine
	{
		_float3		vPosition;
	}VTXLINE_POS;

	typedef struct ENGINE_DLL tagVertexLine_Declaration
	{
		static const unsigned int		iNumElements = 1;
		static D3D11_INPUT_ELEMENT_DESC		Elements[iNumElements];			
	}VTXLINE_POS_DECLARATION;

	typedef struct tagVertexLinePosInstance
	{
		_float4			vRight;
		_float4			vUp;
		_float4			vLook;
		_float4			vPosition;
		_float3			vStartTangent;
		_float3			vEndTangent;
	}VTXLINE_POS_INSTANCE;

	typedef struct ENGINE_DLL tagVertexLinePosInstance_Declaration
	{
		static const unsigned int		iNumElements = 7;
		static D3D11_INPUT_ELEMENT_DESC	Elements[iNumElements];
	}VTXLINE_POS_INSTANCE_DECLARATION;
}