#pragma once
#include "fighter.h"
#include <shitrndr.h>

struct Player : Fighter
{
	static Player* instance;
	float dir = 1;
	bool floppin = 0, flopped = 0;
	float t_flop = 0;

	static void onCollision(CollisionData cd, Body* self, Body* other)
	{
		if(instance->floppin && v2f::dot(cd.normal, v2f(0, -1))<.5)
		{
			instance->floppin = 0;
			instance->flopped = 1;
			instance->parent_set->instantiate(new Attack(instance, instance->pos, instance->a_strength, .3, {3,2}));
		}
	};

	Player(v2f pos_ = {}) : Fighter(pos_)
	{
		instance = this;
		b->onCollision = &onCollision;
	}
	void die() override
	{
		b->tr.pos = {0, 4};
		std::cout << "YOU DIED\n";
		health = health_max;
	}
	void flop()
	{
		if(Platform::instance->onPlatform(b, .5) || t_flop > 0) return;
		b->vel = {0, -25};
		floppin = 1;
		t_flop = 2;
	}
	void move() override
	{
		if(common::inVec().x) dir = common::inVec().x;
		b->vel.x += FD::delta*common::inVec().x*acc;
	}
	void update() override
	{
		if(t_flop>0) t_flop -= FD::delta;
		if(flopped)
		{
			flopped = 0;
			b->vel.y *= .2;
		}
		Fighter::update();
	}
	void render() override
	{
		Fighter::render();
		if(t_flop<=0)
			shitrndr::SetColour({0,255,0,255});
		shitrndr::FillRect(getRect());
	}
};
