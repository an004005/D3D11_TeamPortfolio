#include "stdafx.h"
#include "..\public\JsonStorage.h"
#include "GameUtils.h"

IMPLEMENT_SINGLETON(CJsonStorage)

CJsonStorage::CJsonStorage()
{
}

const Json& CJsonStorage::FindJson(const string& strJsonPath)
{
	lock_guard<mutex> lockGuard(m_mtx);

	const auto itr = m_mapJson.find(strJsonPath);
	if (itr != m_mapJson.end())
		return itr->second;

	return m_EmptyJson;
}

const Json& CJsonStorage::FindOrLoadJson(const string& strJsonPath)
{
	lock_guard<mutex> lockGuard(m_mtx);

	{
		const auto itr = m_mapJson.find(strJsonPath);
		if (itr != m_mapJson.end())
			return itr->second;
	}

	if (CGameUtils::FileExist(CGameUtils::s2ws(strJsonPath).c_str()) 
		&& CGameUtils::CheckExt(strJsonPath.c_str(), {".json"}))
	{
		std::ifstream ifs(strJsonPath);
		m_mapJson.emplace(strJsonPath, Json::parse(ifs));
	}

	{
		const auto itr = m_mapJson.find(strJsonPath);
		if (itr != m_mapJson.end())
			return itr->second;
	}

	return m_EmptyJson;
}

void CJsonStorage::LoadJson(const string& strJsonPath)
{
	lock_guard<mutex> lockGuard(m_mtx);
	if (CGameUtils::FileExist(CGameUtils::s2ws(strJsonPath).c_str()) 
		&& CGameUtils::CheckExt(strJsonPath.c_str(), {".json"}))
	{
		std::ifstream ifs(strJsonPath);
		m_mapJson.emplace(strJsonPath, Json::parse(ifs));
	}
}

void CJsonStorage::SaveJson(const string& strJsonPath)
{
	const auto itr = m_mapJson.find(strJsonPath);
	if (itr == m_mapJson.end())
	{
		IM_LOG("Not Found Json path : %s", strJsonPath.c_str())
		return;
	}

	std::ofstream file(strJsonPath);
	file << m_mapJson.find(strJsonPath)->second;
}

void CJsonStorage::UpdateJson(const string& strJsonPath, const Json& json)
{
	const auto itr = m_mapJson.find(strJsonPath);
	if (itr == m_mapJson.end())
	{
		IM_LOG("Not Found Json path : %s", strJsonPath.c_str())
		return;
	}

	m_mapJson[strJsonPath] = json;
}

void CJsonStorage::Free()
{
	m_mapJson.clear();
}

