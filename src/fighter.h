#pragma once
#include "objects.h"
#include "platform.h"
#include "effects.h"

struct Fighter : FizThing
{
	float max_speed = 5;
	float acc = 15;
	float jump_h = 8;

	float health = 1, health_max = 1, regen_rate = .2;

	float a_strength = .3, a_cooldown = .2;
	float t_ac = 0, t_j = 0;
	
	Fighter(v2f pos_ = {}, v2f scl_ = {1,1}, float mass_ = 1) : FizThing(pos_, scl_, mass_)
	{
		b->bitmask = 0b00000010;
		b->tr.rot = M_PIf32;
	}

	virtual void move()
	{
		jump();
		auto lims = Platform::instance->getLimitsH();
		float d = std::max(lims.first+1, std::min(lims.second-1, (Platform::instance->getBalancePointX()-pos.x)+(common::frand()*2-1)));
		d = d/std::abs(d);
		b->vel.x += FD::delta*d*acc;
		attack(b->vel.normalised()*v2f(1,-1));
	}
	virtual void jump()
	{
		if(t_j>0) return;
		t_j = .2;
		if(!Platform::instance->onPlatform(b, .1)) return;
		b->tr.pos.y += .1;
		b->vel.y = jump_h;
		b->vel.x *= .8;
		Platform::instance->applyForce(b->tr.pos.x, b->mass*.1);
		audio::play(S_JUMP, .4);
	}
	virtual void takeDamage(float d)
	{
		audio::play(S_HIT, .3);
		health -= d;
	}
	virtual void die()
	{
		active = visible = false;
		std::cout << "ouch\n";
	}
	
	struct Attack : Thing2D
	{
		Fighter* parent;
		float strength;
		float life;

		Attack(Fighter* parent_, v2f pos_, float strength_, float life_ = .3, v2f scl_ = {1,1}) : Thing2D(pos_, scl_), parent(parent_), strength(strength_), life(life_) {}
		void render() override {}
		/*
		{
			shitrndr::SetColour({255,120,0,255});
			SDL_Rect r = getRect();
			r.x -= r.w/2;
			r.y -= r.h/2;
			FillRect(r);
		}
		*/
		void update() override
		{
			if(life<=0)
			{
				parent_set->scheduleDestroy(this);
				return;
			}
			for(auto p : parent_set->things_id)
			{
				Fighter* o = dynamic_cast<Fighter*>(p.second);
				if(!o || o==parent) continue;
				if(cumt::aabb::getOverlap(getRect(), o->getRect()))
				{
					o->takeDamage(strength);
					o->b->vel = (o->b->tr.pos-(pos*v2f(1,-1))).normalised()*strength*10/o->b->mass;
					life = 0;
					parent_set->instantiate(new PunchFX(pos-scl, strength, scl));
					break;
				}
			}
			life -= FD::delta;
		}
	};
	void attack(v2f dir)
	{
		if(t_ac>0) return;
		t_ac = a_cooldown;
		parent_set->instantiate(new Attack(this, pos+dir, a_strength));
		audio::play(S_SWING, .1);
	}
	void render() override
	{
		FizThing::render();
		shitrndr::SetColour({0,0,255,255});
		SDL_Rect r = getRect();
		r.y -= 10;
		r.h = 5;
		r.x -= r.w;
		r.w *= 2;
		shitrndr::DrawRect(r);
		r.w = r.w*health/health_max;
		shitrndr::FillRect(r);
	}
	void update() override
	{
		move();
		// limit horizontal speed to allow a momentary acceleration curve
		if(std::abs(b->vel.x)>max_speed)
			b->vel.x = b->vel.x/std::abs(b->vel.x)*max_speed;

		//drag
		b->vel -= b->vel.normalised()*FD::delta*.1;

		if(pos.y>20 || health<=0) die();
		else if(health<health_max) health += FD::delta*regen_rate;

		// timers
		if(t_ac>0) t_ac -= FD::delta;
		if(t_j>0) t_j -= FD::delta;

		FizThing::update();
	}
};
