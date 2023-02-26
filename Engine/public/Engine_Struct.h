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
		enum TYPE { TYPE_DIRECTIONAL, TYPE_POINT, TYPE_END };

		TYPE				eType;

		bool				isEnable;

		XMFLOAT4			vDirection;

		/* ��� ����(�ȼ�)�� �޴� ���� ���⺤�Ͱ� �� �ٸ��� ǥ���Ǿ���ϱ⶧���̴�. */
		/* ��)�̴������� ���� ���⺤�͸� ���ؼ� �����Ѵ�. */
		XMFLOAT4			vPosition;
		float				fRange;		

		XMFLOAT4			vDiffuse;
		XMFLOAT4			vAmbient;
		XMFLOAT4			vSpecular;
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


	typedef struct tagVertexMatrix
	{
		_float4			vRight;
		_float4			vUp;
		_float4			vLook;
		_float4			vPosition;
	}VTXMATRIX;

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


	struct PxMesh
	{
		physx::PxTriangleMesh* pPxMesh = nullptr;
		physx::PxTriangleMeshGeometry tMeshGeom;
		physx::PxShape* pShape = nullptr;
	};
}