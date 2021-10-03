#pragma once
#include <SDL2/SDL_render.h>
#include <cumt/cumt.h>
#include <fizzyx/fizzyx.h>
#include <shitrndr.h>
#include "resources.h"

using namespace cumt;
using namespace fizzyx;
using namespace shitrndr;

struct FizThing : Thing2D
{
	static std::map<Body*, FizThing*> lookup;
	static constexpr float MAX_VEL_Y = 10;

	static World* wld;
	Body* b;

	FizThing(v2f pos_ = {}, v2f scl_ = {1,1}, float mass_ = 1) : Thing2D(pos_, scl_)
	{
		b = new Body(pos_, mass_);
		lookup[b] = this;
		b->tr.scl = scl_;
		b->tr.pos.y *= -1;
		b->col = new PolygonCollider();
		b->col->tr = &b->tr;
		wld->addBody(b);
	}
	~FizThing()
	{
		wld->removeBody(b);
		lookup.erase(b);
		delete b;
	}
	inline v2f getRight() { return v2f(-std::cos(b->tr.rot), std::sin(b->tr.rot)); }
	inline v2f getUp() { return v2f(-std::cos(b->tr.rot+M_PI_2f32), std::sin(b->tr.rot+M_PI_2f32)); }
	SDL_Rect getGoodRect()
	{
		SDL_Rect r = getRect();
		r.x -= r.w/2;
		r.y -= r.h/2;
		return r;
	}
	void update() override;
	void render() override;
};
