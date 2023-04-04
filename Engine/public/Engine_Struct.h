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

	/* ���⼺����, ������ : ���⺤�Ͱ� �ʿ��ϴ�. */
	typedef struct tagLightDesc
	{
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_FOV, TYPE_CAPSULE, TYPE_END };

		TYPE				eType;

		bool				isEnable;

		_float4			vDirection;

		/* ��� ����(�ȼ�)�� �޴� ���� ���⺤�Ͱ� �� �ٸ��� ǥ���Ǿ���ϱ⶧���̴�. */
		/* ��)�̴������� ���� ���⺤�͸� ���ؼ� �����Ѵ�. */
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
		XMUINT4			vBlendIndex; /* �� ������ ����ؾ��ϴ� ���� �ε���(��ü�� ���� ����(x), �� ������ �Ҽӵ� �޽ÿ� ������ �ִ� ������ ����(o))  */
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
	// 	_float3 vMinPos; //�ּ� ��ġ
	// 	_float3 vMaxPos; //�ִ� ��ġ
	//
	// 	_float fMaxSpeed; //�ִ� �ӷ�
	// 	_float fMinSpeed; //�ּ� �ӷ�
	// 	_float3 vMaxDir; //����(�븻������)
	// 	_float3 vMinDir; //����(�븻������)
	// 	_float3 vAcceleration; //�ӵ� ��ȭ��
	//
	// 	_float3 vMinScale; //�ּ� ũ��
	// 	_float3 vMaxScale; //�ִ� ũ��
	// 	_float3 vScaleVariation; //ũ�� ��ȭ��
	//
	// 	_float4 vMinColor; //�ּ� Į��
	// 	_float4 vMaxColor; //�ִ� Į��
	// 	_float4 vColorVariation; //�� ��ȭ��
	//
	// 	_float fMinLifeTime; //�ּ� �����ð�
	// 	_float fMaxLifeTime; //�ִ� �����ð�
	//
	// 	_float fGenerateRate; //��ƼŬ ���� ����(�ʴ� ��ƼŬ ��)
	// 	_float fGenerationTime; //��ƼŬ ���� �ð�(���ʱ��� ������ ������)
	// 	_uint  iTotalCnt;//��ü ��ƼŬ ����
	//
	// }PARTICLEDESC;

	// typedef struct Particle
	// {
	// 	_float3 vPos;
	// 	_float4	vColor;
	// 	_float3	vSize;
	// 	_float3 vVelocity;
	// 	_float fLifeTime;
	// 	_float fAge; //���� ��ƼŬ �����ð�
	// 	_uint iNum; //�ν�ź�� ��ȣ
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