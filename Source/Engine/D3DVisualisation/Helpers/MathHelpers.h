#pragma once

namespace SpringWindEngine
{
	constexpr UINT RoundUpMultiple16(const UINT numToRound)
	{
		return (numToRound + 15) & ~(15); // Only works when multiple is a power of 2
	}

	inline bool XMFloat4Equals(const XMFLOAT4& a, const XMFLOAT4& b)
	{
		return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
	}

	inline bool operator!=(const XMFLOAT2& a, const XMFLOAT2& b)
	{
		return (a.x != b.x) || (a.y != b.y);
	}

	template <class T>
	inline void hash_combine(std::size_t& seed, const T& v)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

}