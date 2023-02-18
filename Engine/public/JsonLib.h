#pragma once
#include "nlohmann/json.hpp"
#include <fstream>

namespace nlohmann
{
	template <>
	struct adl_serializer<Engine::_float2>
	{
		static void to_json(json& j, const Engine::_float2& value)
		{
			j = json{ value.x, value.y };
		}

		static void from_json(const json& j, Engine::_float2& value)
		{
			value.x = j.at(0);
			value.y = j.at(1);
		}
	};

	template <>
	struct adl_serializer<ImVec2>
	{
		static void to_json(json& j, const ImVec2& value)
		{
			j = json{ value.x, value.y };
		}

		static void from_json(const json& j, ImVec2& value)
		{
			value.x = j.at(0);
			value.y = j.at(1);
		}
	};

	template <>
	struct adl_serializer<Engine::_float3>
	{
		static void to_json(json& j, const Engine::_float3& value)
		{
			j = json{ value.x, value.y, value.z };
		}

		static void from_json(const json& j, Engine::_float3& value)
		{
			value.x = j.at(0);
			value.y = j.at(1);
			value.z = j.at(2);
		}
	};

	template <>
	struct adl_serializer<Engine::_float4>
	{
		static void to_json(json& j, const Engine::_float4& value)
		{
			j = json{ value.x, value.y, value.z, value.w };
		}

		static void from_json(const json& j, Engine::_float4& value)
		{
			value.x = j.at(0);
			value.y = j.at(1);
			value.z = j.at(2);
			value.w = j.at(3);
		}
	};

	template <>
	struct adl_serializer<XMFLOAT2>
	{
		static void to_json(json& j, const XMFLOAT2& value)
		{
			j = json{ value.x, value.y };
		}

		static void from_json(const json& j, XMFLOAT2& value)
		{
			value.x = j.at(0);
			value.y = j.at(1);
		}
	};

	template <>
	struct adl_serializer<XMFLOAT3>
	{
		static void to_json(json& j, const XMFLOAT3& value)
		{
			j = json{ value.x, value.y, value.z };
		}

		static void from_json(const json& j, XMFLOAT3& value)
		{
			value.x = j.at(0);
			value.y = j.at(1);
			value.z = j.at(2);
		}
	};

	template <>
	struct adl_serializer<XMFLOAT4>
	{
		static void to_json(json& j, const XMFLOAT4& value)
		{
			j = json{ value.x, value.y, value.z, value.w };
		}

		static void from_json(const json& j, XMFLOAT4& value)
		{
			value.x = j.at(0);
			value.y = j.at(1);
			value.z = j.at(2);
			value.w = j.at(3);
		}
	};

	template <>
	struct adl_serializer<XMUINT4>
	{
		static void to_json(json& j, const XMUINT4& value)
		{
			j = json{ value.x, value.y, value.z, value.w };
		}

		static void from_json(const json& j, XMUINT4& value)
		{
			value.x = j.at(0);
			value.y = j.at(1);
			value.z = j.at(2);
			value.w = j.at(3);
		}
	};

	template <>
	struct adl_serializer<KEYFRAME>
	{
		static void to_json(json& j, const KEYFRAME& value)
		{
			j["Time"] = value.Time;
			j["Position"] = value.vPosition;
			j["Rotation"] = value.vRotation;
			j["Scale"] = value.vScale;
		}

		static void from_json(const json& j, KEYFRAME& value)
		{
			j["Time"].get_to(value.Time);
			j["Position"].get_to(value.vPosition);
			j["Rotation"].get_to(value.vRotation);
			j["Scale"].get_to(value.vScale);
		}
	};
	

	template <>
	struct adl_serializer<Engine::_float4x4>
	{
		static void to_json(json& j, const Engine::_float4x4& value)
		{
			for (int col = 0; col < 4; ++col)
			{
				for (int row = 0; row < 4; ++row)
				{
					j.push_back(value.m[col][row]);
				}
			}
		}

		static void from_json(const json& j, Engine::_float4x4& value)
		{
			for (int col = 0; col < 4; ++col)
			{
				for (int row = 0; row < 4; ++row)
				{
					value.m[col][row] = j.at(col * 4 + row);
				}
			}
		}
	};

	template <>
	struct adl_serializer<Engine::FACEINDICES32>
	{
		static void to_json(json& j, const Engine::FACEINDICES32& value)
		{
			j = json{ value._0, value._1, value._2 };
		}

		static void from_json(const json& j, Engine::FACEINDICES32& value)
		{
			value._0 = j.at(0);
			value._1 = j.at(1);
			value._2 = j.at(2);
		}
	};
}
