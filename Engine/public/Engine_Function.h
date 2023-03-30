#pragma once

namespace Engine
{
	class CTag_Finder
	{
	public:
		explicit CTag_Finder(const wchar_t* pTag)
			: m_pTargetTag(pTag)
		{
		}
		~CTag_Finder() {		}
	public:
		template<typename T>
		bool operator()(const T& pair)
		{
			if (0 == lstrcmpW(m_pTargetTag, pair.first))
			{
				return true;
			}

			return false;
		}

	private:
		const wchar_t*		m_pTargetTag = nullptr;
	};


	template <typename T>
	void Safe_Delete(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete pPointer;
			pPointer = nullptr;
		}
	}

	template <typename T>
	void Safe_Delete_Array(T& pPointer)
	{
		if (nullptr != pPointer)
		{
			delete [] pPointer;
			pPointer = nullptr;
		}
	}

	template <typename T>
	unsigned long Safe_AddRef(T& pInstance)
	{
		unsigned long	dwRefCnt = 0;

		if (nullptr != pInstance)		
			dwRefCnt = pInstance->AddRef();		

		return dwRefCnt;
	}

	template <typename T>
	unsigned long Safe_Release(T& pInstance) 
	{
		unsigned long	dwRefCnt = 0;
		if (nullptr != pInstance)
		{
			dwRefCnt = pInstance->Release();
			if (0 == dwRefCnt)
				pInstance = nullptr;
		}

		return dwRefCnt;
	}



	inline void wc2c(const wchar_t* szWchar, char* szChar)
	{
		_int len;
		_int slength = lstrlen(szWchar) + 1;
		len = ::WideCharToMultiByte(CP_ACP, 0, szWchar, slength, 0, 0, 0, 0);
		::WideCharToMultiByte(CP_ACP, 0, szWchar, slength, szChar, len, 0, 0);
	}

	inline void c2wc(const char* szChar, wchar_t* szWchar)
	{
		_int len;
		_int slength = static_cast<_int>(strlen(szChar)) + 1;
		len = ::MultiByteToWideChar(CP_ACP, 0, szChar, slength, 0, 0);
		::MultiByteToWideChar(CP_ACP, 0, szChar, slength, szWchar, len);
	}

	inline wstring s2ws(const string& s)
	{
		_int len;
		_int slength = static_cast<_int>(s.length()) + 1;
		len = ::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		::MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
		wstring ret(buf);
		delete[] buf;
		return ret;
	}

	inline string ws2s(const wstring& s)
	{
		_int len;
		_int slength = static_cast<_int>(s.length()) + 1;
		len = ::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, 0, 0, 0, 0);
		string r(len, '\0');
		::WideCharToMultiByte(CP_ACP, 0, s.c_str(), slength, &r[0], len, 0, 0);
		return r;
	}

	enum class ECOLLISION_TYPE
	{
		COLL_BLOCK,
		COLL_IGNORE,
		COLL_END
	};

	enum ECOLLIDER_TYPE
	{
		CT_PLAYER,
		CT_MONSTER,
		CT_PLAYER_ATTACK,
		CT_MONSTER_ATTACK,
		CT_PSYCHICK_OBJ,
		CT_TRIGGER_FOR_PLAYER,
		CT_TRIGGER_FOR_MONSTER,
		CT_STATIC,
		CT_MONSTER_PART,
		CT_MONSTER_RANGE,
		CT_PLAYER_RANGE,
		CT_PARTICLE,
		CT_END
	};

	enum ECOLLIDER_TYPE_BIT
	{
		CTB_PLAYER = 1 << CT_PLAYER,
		CTB_MONSTER = 1 << CT_MONSTER,
		CTB_PLAYER_ATTACK = 1 << CT_PLAYER_ATTACK,
		CTB_MONSTER_ATTACK = 1 << CT_MONSTER_ATTACK,
		CTB_PSYCHICK_OBJ = 1 << CT_PSYCHICK_OBJ,
		CTB_TRIGGER_FOR_PLAYER = 1 << CT_TRIGGER_FOR_PLAYER,
		CTB_TRIGGER_FOR_MONSTER = 1 << CT_TRIGGER_FOR_MONSTER,
		CTB_STATIC = 1 << CT_STATIC,
		CTB_MONSTER_PART = 1 << CT_MONSTER_PART,
		CTB_MONSTER_RANGE = 1 << CT_MONSTER_RANGE,
		CTB_PLAYER_RANGE = 1 << CT_PLAYER_RANGE,
		CTB_PARTICLE = 1 << CT_PARTICLE,
	};

	inline ECOLLIDER_TYPE_BIT GetCollTypeBit(ECOLLIDER_TYPE eType)
	{
		static array<ECOLLIDER_TYPE_BIT, CT_END> collTypeBits
		{
			CTB_PLAYER, CTB_MONSTER, CTB_PLAYER_ATTACK, CTB_MONSTER_ATTACK, CTB_PSYCHICK_OBJ,
			CTB_TRIGGER_FOR_PLAYER, CTB_TRIGGER_FOR_MONSTER, CTB_STATIC, CTB_MONSTER_PART, CTB_MONSTER_RANGE, CTB_PLAYER_RANGE, CTB_PARTICLE
		};

		return collTypeBits[eType];
	}

	enum ESIMULATION_PRESET
	{
		SI_CONTROLLER = CTB_PLAYER | CTB_MONSTER | CTB_PSYCHICK_OBJ | CTB_STATIC
	};


	struct RayCastParams
	{
		OUT physx::PxRaycastBuffer* rayOut;
		_float4 vOrigin;
		_float4 vDir;
		_float fDistance;
		_uint iTargetType;
		_bool bSingle = true;
		_float fVisibleTime = 1.f;
		physx::PxQueryFlags queryFlags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC;
	};

	struct SphereOverlapParams
	{
		OUT physx::PxOverlapBuffer* overlapOut;
		_float fRadius;
		_float4 vPos;
		_uint iTargetType;
		_float fVisibleTime = 1.f;
		physx::PxQueryFlags queryFlags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC;
	};

	struct CapsuleOverlapParams
	{
		OUT physx::PxOverlapBuffer* overlapOut;
		_float fRadius;
		_float fHalfHeight;
		_float3 vLook;
		_float4 vPos;
		_uint iTargetType;
		_float fVisibleTime = 1.f;
		physx::PxQueryFlags queryFlags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC;
	};

	struct SphereSweepParams
	{
		OUT physx::PxSweepBuffer* sweepOut;
		_float fRadius;
		_float4 vPos;
		_float3 vUnitDir;
		_float fDistance;
		_uint iTargetType;
		_float fVisibleTime = 1.f;
		physx::PxQueryFlags queryFlags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC;
	};

	struct CapsuleSweepParams
	{
		OUT physx::PxSweepBuffer* sweepOut;
		_float fRadius;
		_float fHalfHeight;
		_float3 vLook;
		_float4 vPos;
		_float3 vUnitDir;
		_float fDistance;
		_uint iTargetType;
		_float fVisibleTime = 1.f;
		physx::PxQueryFlags queryFlags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC;
	};

	struct PxCapsuleSweepParams
	{
		OUT physx::PxSweepBuffer* sweepOut;
		physx::PxCapsuleGeometry CapsuleGeo;
		physx::PxTransform pxTransform;
		_float3 vUnitDir;
		_float fDistance;
		_uint iTargetType;
		_float fVisibleTime = 1.f;
		physx::PxQueryFlags queryFlags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC;
	};


	struct PxCapsuleOverlapParams
	{
		OUT physx::PxOverlapBuffer* overlapOut;
		physx::PxCapsuleGeometry CapsuleGeo;
		physx::PxTransform pxTransform;
		_uint iTargetType;
		_float fVisibleTime = 1.f;
		physx::PxQueryFlags queryFlags = physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eSTATIC;
	};
}