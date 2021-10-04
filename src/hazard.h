#pragma once
#include "objects.h"
#include "fighter.h"
#include <cumt/cumt_audio.h>

void shakeCam(float, float);
struct Hazard : FizThing
{
	Hazard(float x, float r = 1, float mass_ = 4) : FizThing(v2f(x, -10), v2f(r*2,r*2), mass_)
	{
		delete b->col;
		b->col = new CircleCollider({}, r);
		b->col->tr = &b->tr;
		b->vel.y = -20;
		b->onCollision = [](CollisionData cd, Body* self, Body* other)
		{
			Fighter* f = dynamic_cast<Fighter*>(FizThing::lookup[other]);
			if(f) f->takeDamage(.8);
			shakeCam(.2, .5);
			audio::play(S_HAZARD_HIT, .3);
		};
		b->postCollision = [](CollisionData cd, Body* self, Body* other)
		{
			self->passive = 1;
		};
	}
	void update() override
	{
		if(pos.y>20) active = visible = false;
		FizThing::update();
	}
	void render() override
	{
		SDL_Rect r = getGoodRect();
		if(b->passive) r.w = r.h = r.w*.8;
		SDL_RenderCopyEx(ren, T_HAZARD, 0, &r, 90*b->vel.getLength(), 0, b->passive?SDL_FLIP_NONE:SDL_FLIP_HORIZONTAL);
	}
};
