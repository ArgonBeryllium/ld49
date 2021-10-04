#pragma once
#include "fighter.h"
#include "effects.h"
#include <cumt/cumt_audio.h>
#include <cumt/cumt_common.h>
#include <shitrndr.h>

struct Player : Fighter
{
	static Player* instance;
	float dir = 1;
	bool floppin = 0, flopped = 0;
	float t_flop = 0;

	SDL_Texture* tex = T_P_IDLE;

	static void onCollision(CollisionData cd, Body* self, Body* other)
	{
		if(instance->floppin && v2f::dot(cd.normal, v2f(0, -1))>0)
		{
			instance->floppin = 0;
			instance->flopped = 1;
			instance->parent_set->instantiate(new Attack(instance, instance->pos, instance->a_strength*2, .3, {3,2}));
			instance->parent_set->instantiate(new PunchFX(instance->pos, instance->a_strength*2, {3,2}));
			audio::play(S_HAZARD_HIT, .3);
			shakeCam(.3, .3);
		}
	};

	Player(v2f pos_ = {}) : Fighter(pos_)
	{
		instance = this;
		b->onCollision = &onCollision;
		b->bitmask = 0b00000001;
	}

	void takeDamage(float d) override
	{
		shakeCam(d);
		health -= d;
		audio::play(S_HIT);
	}
	void die() override;
	void flop()
	{
		if(Platform::instance->onPlatform(b, 1) || t_flop > 0 || floppin) return;
		b->vel = {0, -50};
		floppin = 1;
		t_flop = 2;
	}
	void move() override
	{
		if(common::inVec().x)
			dir = common::inVec().x;
		float d =  FD::delta*common::inVec().x*acc;
		if(Platform::instance->onPlatform(b)) b->vel += d*getRight();
		else b->vel.x += d*.5;
	}
	void update() override
	{
		if(t_flop>0) t_flop -= FD::delta;
		else if (floppin) floppin = 0;
		if(flopped)
		{
			flopped = 0;
			b->vel.y *= .2;
		}
		Fighter::update();
	}
	void render() override
	{
		if(floppin) tex = T_P_FALL;
		else if(Input::getKey(SDLK_x)) tex = T_P_PUNCH;
		else if(!Platform::instance->onPlatform(b)) tex = T_P_AIRBORNE;
		else if(common::inVec().x) tex = T_P_RUN[int(FD::time*4)%2];
		else tex = T_P_IDLE;
		SDL_Rect r = getRect();
		r.y -= r.h/2;
		r.x -= r.w/2;
		r.w *= 1.5;
		RenderCopyEx(ren, tex, {0,0,48,32}, r, -b->tr.rot*180/M_PIf32+180, {16,16}, dir>0?SDL_FLIP_NONE:SDL_FLIP_HORIZONTAL);
		std::string hpstr = "[";
		for(int i = 0; i != 16; i++)
			hpstr+= (i/16.f<health/health_max)?'=':' ';
		hpstr += "]";
		render::text({WindowProps::getWidth()/2, 5}, hpstr, TD_DEF_C);
		render::text({WindowProps::getWidth()/2, 15}, t_flop<=0?"RDY":" ", TD_DEF_C);
	}
};
