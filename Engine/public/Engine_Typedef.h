#pragma once

namespace Engine
{
	typedef		signed char				_byte;
	typedef		unsigned char			_ubyte;

	typedef		wchar_t					_tchar;

	typedef		signed short			_short;
	typedef		unsigned short			_ushort;

	typedef		signed int				_int;
	typedef		unsigned int			_uint;

	typedef		signed long				_long;
	typedef		unsigned long			_ulong;

	typedef		float					_float;
	typedef		double					_double;

	typedef		bool					_bool;

	typedef		SimpleMath::Vector2				_float2;
	typedef		SimpleMath::Vector3				_float3;
	typedef		SimpleMath::Vector4				_float4;
	typedef		XMVECTOR				_vector;
	typedef		FXMVECTOR				_fvector;
	typedef		CXMVECTOR				_cvector;
	typedef		HXMVECTOR				_hvector;
	typedef		GXMVECTOR				_gvector;

	typedef		SimpleMath::Matrix				_float4x4;
	typedef		XMMATRIX				_matrix;
	typedef		FXMMATRIX				_fmatrix;
	typedef		CXMMATRIX				_cmatrix;

	struct cmp_char
	{
	   bool operator()(const char* a, const char* b) const
	   {
	      return std::strcmp(a, b) < 0;
	   }
	};
	template<typename VALUE>
	using cmap = map<const char*, VALUE, cmp_char>;

	struct cmp_wchar
	{
	   bool operator()(const _tchar* a, const _tchar* b) const
	   {
	      return lstrcmp(a, b) < 0;
	   }
	};
	template<typename VALUE>
	using wcmap = map<const _tchar*, VALUE, cmp_wchar>;
}
