#include "stdafx.h"
#include "..\public\RefGuard.h"
using namespace Engine;

template <typename T>
CRefGuard<T>::CRefGuard(CRefGuard<T>& RefGuard)
{
	m_pBase = RefGuard.get();
	m_pBase->AddRef();
}

template <typename T>
CRefGuard<T>::CRefGuard(T*&& pBase)
{
	m_pBase = std::forward<T*>(pBase);
	m_pBase->AddRef();
}

template <typename T>
CRefGuard<T>::~CRefGuard()
{
	m_pBase->Release();
	m_pBase = nullptr;
}