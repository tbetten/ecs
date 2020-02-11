#include "component.h"
#include "system_manager.h"
#include "messaging.h"

#include <algorithm>
#include <iostream>
#include <type_traits>
#include <SFML/System.hpp>

using namespace ecs;

void System_manager::setup_events ()
{
//	m_entity_mgr->get_event ().bind ([this](auto p) {entity_modified (p); });
	std::cout << "in sys mgr setup events\n";
	m_messenger->bind("entity_modified", [this](auto p) {entity_modified(p); });
	for (auto& system : m_systems)
	{
		system.second->setup_events ();
	}
}

void System_manager::add_entity_manager (Entity_manager* mgr)
{
	m_entity_mgr = mgr;
}

Entity_manager* System_manager::get_entity_mgr ()
{
	return m_entity_mgr;
}

void System_manager::set_context (Shared_context* context)
{
	m_context = context;
}

Shared_context* System_manager::get_context ()
{
	return m_context;
}

void System_manager::add_system (System_type id, S_base::Ptr system)
{
	m_systems[id] = std::move (system);
}

void System_manager::update (sf::Int64 dt)
{
	//std::for_each (m_systems.begin (), m_systems.end (), [dt](auto& entry) {entry.second->update (dt); });
	for (auto& sys : m_systems)
	{
		sys.second->update(dt);
	}
}

void System_manager::entity_modified (std::any val)
{
	Modified_payload p;
	try
	{
		p = std::any_cast<Modified_payload>(val);
	}
	catch (const std::bad_any_cast& e)
	{
		std::cout << e.what () << std::endl;
		return;
	}
	auto x = p.m_bits;
	std::cout << "entity modified: sys mgr; entity = " + std::to_string(p.m_entity) + "\t" + p.m_bits.to_string() +  "\n";
	for (auto& sys : m_systems)
	{
		auto& system = sys.second;
		if (system->fits_requirements (p.m_bits))
		{
			if (!system->has_entity (p.m_entity))
			{
				system->add_entity (p.m_entity);
			}
		}
		else
		{
			if (system->has_entity (p.m_entity))
			{
				system->remove_entity (p.m_entity);
			}
		}
	}
}