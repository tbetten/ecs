#include "component.h"

#include <algorithm>
#include <numeric>
#include <string>

using namespace ecs;

void Entity_manager::add_component (Component_type component_type, C_base::Ptr component)
{
	while (m_components.size() <= static_cast<size_t>(component_type))
	{
		m_components.push_back(nullptr);
	}
	m_components[static_cast<size_t>(component_type)] = std::move(component);
}

Entity_manager::Entity_manager(messaging::Messenger* messenger) : messaging::Sender{ messenger }
{
	add_message("entity_modified");
}

std::optional<size_t> Entity_manager::get_index(Component_type component, Entity_id entity) const
{
	return m_entity_index[entity][static_cast<size_t>(component)];
}

std::optional<size_t> Entity_manager::take_free_index(size_t component_index)
{
	auto& free_indices = m_components[component_index]->m_free_indices;
	if (free_indices.empty()) return std::nullopt;
	size_t index = free_indices.back();
	free_indices.erase(--free_indices.end());
	return index;
}

std::optional<Entity_id> Entity_manager::idex_to_entity(Component_type c_id, std::size_t index) const
{
	auto itr = std::find_if(std::cbegin(m_entity_index), std::cend(m_entity_index), [c_id, index](Component_indices row) {return *(row.at(static_cast<std::size_t>(c_id))) == index; });
	if (itr == std::cend(m_entity_index))
	{
		return std::nullopt;
	}
	else
	{
		return std::distance(std::cbegin(m_entity_index), itr);
	}
}

bool Entity_manager::has_component(Entity_id entity, Component_type component) const
{
	return m_entity_index[entity][static_cast<size_t>(component)].has_value();
}

void Entity_manager::do_notify(Entity_id entity) const
{
	Bitmask b;
	int i{ 0 };
	for (auto index : m_entity_index[entity])
	{
		if (index) b.set(i);
		++i;
	}
	notify("entity_modified", Modified_payload{ entity, b });
}

C_base* Entity_manager::get_component_by_id (size_t id) const
{
	return m_components [id].get ();
}

bool Entity_manager::add_component_to_entity(Entity_id entity, Component_type component, std::string key, bool will_notify)
{
	size_t comp_index = static_cast<size_t>(component);
	bool loaded { true };
	if (m_entity_index[entity][comp_index])  // already have it
	{
		return loaded;
	}

	auto& comp = m_components [comp_index];
	auto& shares = comp->m_shares;
	std::optional<size_t> new_index { std::nullopt };
	if (key != "")
	{
		auto share_itr = std::find_if (std::cbegin(shares), std::cend(shares), [&key] (const Share& s){return s.key == key; });
		if (share_itr != std::cend (shares))
		{
			new_index = share_itr->component_index;
		}
	}
	if (!new_index) new_index = take_free_index (comp_index);
	if (!new_index)
	{
		new_index = m_components [comp_index]->get_size ();
		m_components [comp_index]->add_row ();
		loaded = false;
	}
	if (!new_index)
	{
		throw "panic!!!";
	}
	m_entity_index [entity][comp_index] = new_index;
	if (key != "") shares.emplace_back (key, entity, new_index.value());
	if (will_notify) do_notify (entity);
	return loaded;


/*	auto free_index = take_free_index(comp_index);
	if (!free_index)
	{
		auto size = m_components[comp_index]->get_size();
		m_entity_index[entity][comp_index] = size;
		m_components[comp_index]->add_row();
		return true;
	}
	m_entity_index[entity][comp_index] = free_index;
	if (will_notify)
	{
		do_notify(entity);
	}
	return true;*/
}

bool Entity_manager::remove_component_from_entity(Entity_id entity, Component_type component, bool will_notify)
{
	size_t comp_index = static_cast<size_t>(component);
	auto index = m_entity_index[entity][comp_index];
	if (!index) return false;  // don't have it
	
	auto& comp = m_components [comp_index];
	auto& shares = comp->m_shares;
	auto s = std::find_if (std::begin (shares), std::end (shares), [entity] (const Share& s){return s.entity == entity; });
	if (s != std::end (shares))
	{
		auto key = s->key;
		shares.erase (s);
		auto s2 = std::find_if (std::cbegin (shares), std::cend (shares), [&key] (const Share& s){return s.key == key; });
		if (s2 == std::cend (shares))
		{
			m_components [comp_index]->m_free_indices.push_back (index.value ());
		}
		m_entity_index [entity][comp_index] = std::nullopt;
		return true;
	}

	m_components[comp_index]->m_free_indices.push_back(index.value());
	m_entity_index[entity][comp_index] = std::nullopt;
	if (will_notify) do_notify(entity);
	return true;
}

Entity_id Entity_manager::add_entity(Bitmask b, std::string key)
{
	Entity_id new_id{};
	auto first_free_itr = std::find_if(m_entity_index.cbegin(), m_entity_index.cend(), [](Component_indices row)
		{return std::find_if(row.cbegin(), row.cend(), [](Component_index index) {return index != std::nullopt; }) == row.cend(); });
	if (first_free_itr == m_entity_index.end())  // no unused slot
	{
		new_id = m_entity_index.size();
		Component_indices new_row(max_components, std::nullopt);
		m_entity_index.push_back(new_row);
	}
	else
	{
		new_id = std::distance(m_entity_index.cbegin(), first_free_itr);
	}
	for (size_t i = 0; i < b.size(); ++i)
	{
		if (b[i])
		{
			add_component_to_entity(new_id, static_cast<Component_type>(i), key, false);
		}
	}
	notify("entity_modified", Modified_payload{ new_id, b });
//	m_entity_modified.notify(Modified_payload{ new_id, b });
	return new_id;
}

/*void Entity_manager::update_entity(Entity_id id, Bitmask b)
{
	for (size_t i = 0; i < b.size(); ++i)
	{
		auto comp = static_cast<Component_type>(i);
		if (!b[i] && has_component(id, comp))
		{
			remove_component_from_entity(id, comp, false);
		}
		if (b[i] && !has_component(id, comp))
		{
			add_component_to_entity(id, comp, false);
		}
	}
	notify("entity_modified", Modified_payload{ id, b });
//	m_entity_modified.notify(Modified_payload{ id, b });
}*/

void Entity_manager::remove_entity(Entity_id id)
{
	auto row = m_entity_index[id];
	size_t i{ 0 };
	std::for_each(row.begin(), row.end(), [&i, id, this](std::optional<size_t> index) 
		{
			if (index)
			{
				remove_component_from_entity(id, static_cast<Component_type>(i), false);
				index = std::nullopt;
			}
			++i;
		});
	notify("entity_modified", Modified_payload{ id, Bitmask{} });
//	m_entity_modified.notify(Modified_payload{ id, Bitmask{} });
}

