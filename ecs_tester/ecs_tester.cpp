// ecs_tester.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <memory>
#include "ecs.h"

namespace ecs
{
	enum class Component_type : size_t {A, B, C};
}

struct A
{
	void reset() { i = 0; }
	int i;
};

struct B
{
	void reset() { f = 3.14f; }
	float f;
};

struct C
{
	void reset() { s = "hello world"; }
	std::string s;
};

int main()
{
	ecs::Entity_manager em{};
	em.add_component(ecs::Component_type::C, std::unique_ptr<ecs::C_base>(new ecs::Component<C>()));
	em.add_component(ecs::Component_type::A, std::unique_ptr<ecs::C_base>(new ecs::Component<A>()));
	em.add_component(ecs::Component_type::B, std::unique_ptr<ecs::C_base>(new ecs::Component<B>()));
	ecs::Bitmask b1{ 3 };
	auto e1 = em.add_entity(b1);
	ecs::Bitmask b2{ 1 };
	auto e2 = em.add_entity(b2);
	em.add_component_to_entity(e2, ecs::Component_type::C);
	em.remove_component_from_entity(e1, ecs::Component_type::A);
	ecs::Bitmask b3{ 7 };
	auto e3 = em.add_entity(b3);
	em.remove_entity(e1);
	auto e4 = em.add_entity(b3);
}

