#pragma once
#include "objects.h"
#include "fighter.h"

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
			if(f) f->takeDamage(.5);
		};
		b->postCollision = [](CollisionData cd, Body* self, Body* other)
		{
			self->passive = 1;
		};
	}
	void update() override
	{
		if(pos.y>20) parent_set->scheduleDestroy(this);
		FizThing::update();
	}
	void render() override
	{
		SetColour({static_cast<Uint8>(b->passive?55:255),0,0,255});
		v2i pp = spaceToScr(pos-scl/2);
		FillCircle(pp.x, pp.y, getScalar()*scl.x);
	}
};
