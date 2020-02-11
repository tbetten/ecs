#pragma once

#include "system.h"

#include <algorithm>
#include <iostream>

using namespace ecs;

bool S_base::has_entity (Entity_id entity) const
{
	return std::find (m_entities.cbegin (), m_entities.cend (), entity) != m_entities.cend ();
}

bool S_base::add_entity (Entity_id entity)
{
	if (has_entity (entity)) return false;
	m_entities.emplace_back (entity);
	return true;
}

bool S_base::remove_entity (Entity_id entity)
{
	std::cout << "system " << static_cast<int>(m_id) << " remove entity " << entity << "\n";
	if (!has_entity (entity)) return false;
	auto itr = std::find (m_entities.begin (), m_entities.end (), entity);
	m_entities.erase (itr);
	return true;
}

bool S_base::fits_requirements (Bitmask b) const
{
	return std::find_if (m_requirements.cbegin (), m_requirements.cend (), [b](Bitmask mask) {return (b & mask) == mask; }) != m_requirements.cend();
}