#pragma once

#include <bitset>

constexpr std::size_t max_components{ 32u };
namespace ecs
{
	using Entity_id = unsigned int;
	using Bitmask = std::bitset<max_components>;
}
	struct Modified_payload
	{
		ecs::Entity_id m_entity;
		ecs::Bitmask m_bits;
	};
