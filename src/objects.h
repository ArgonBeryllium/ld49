#pragma once
#include <cumt/cumt.h>
#include <fizzyx/fizzyx.h>

using namespace cumt;
using namespace fizzyx;

struct FizThing : Thing2D
{
	static World* wld;
	Body* b;
	FizThing(v2f pos_ = {}, v2f scl_ = {1,1}, float mass_ = 1) : Thing2D(pos_, scl_)
	{
		b = new Body(pos_, mass_);
		b->tr.scl = scl_;
		b->tr.pos.y *= -1;
		b->col = new PolygonCollider();
		b->col->tr = &b->tr;
		wld->addBody(b);
	}
	inline v2f getRight() { return v2f(std::cos(b->tr.rot), std::sin(b->tr.rot)); }
	inline v2f getUp() { return v2f(std::cos(b->tr.rot+M_PI_2f32), std::sin(b->tr.rot+M_PI_2f32)); }
	void update() override;
	void render() override;
};

struct Platform : FizThing
{
	static Platform* instance;
	Platform() : FizThing({0, 2}, {12, 2}, 0) { instance = this; }
	float av = 0;

	inline bool withinH(const Body* o)
	{
		float lb = b->tr.transformPoint(((PolygonCollider*)b->col)->verticies[0]).x;
		float rb = b->tr.transformPoint(((PolygonCollider*)b->col)->verticies[1]).x;
		return o->tr.pos.x > lb && o->tr.pos.x < rb;
	}
	inline bool withinV(const Body* o)
	{
		constexpr float EPSILON = .01;
		b->tr.scl.y += EPSILON;
		CollisionData cd = b->col->testCollision(o->col);
		b->tr.scl.y -= EPSILON;
		return cd.colliding && v2f::dot(cd.normal, v2f(0,1))>0;
	}
	inline bool onPlatform(const Body* o) { return withinH(o) && withinV(o); }
	void update() override
	{
		float wd = 0;
		float ms = 0;
		for(Body* o : b->world->bodies)
		{
			if(o==b) continue;
			if(onPlatform(o))
			{
				wd += o->vel.getLength()*std::max(0.f, v2f::dot(o->vel, getUp()*-1))*o->mass*o->tr.pos.x;
				ms += o->mass;
			}
		}
		if(wd != 0)
		{
			wd /= b->world->bodies.size();
			av += ms*-wd*FD::delta;
		}
		b->tr.rot += av*FD::delta;

		constexpr float ANGLE_LIMIT = M_PIf32*.4;
		if(b->tr.rot>ANGLE_LIMIT)
		{
			b->tr.rot = ANGLE_LIMIT;
			av = -std::abs(av)*.9;
		}
		else if(b->tr.rot<-ANGLE_LIMIT)
		{
			b->tr.rot = -ANGLE_LIMIT;
			av = std::abs(av)*.9;
		}

		av *= 1-FD::delta*2;
	}
};
struct Player : FizThing
{
	static constexpr float MAX_SPEED = 5;
	float acc = 15;

	static Player* instance;

	Player(v2f pos_ = {}) : FizThing(pos_) { instance = this; }
	void update() override
	{
		b->vel.x += FD::delta*common::inVec().x*acc;
		if(std::abs(b->vel.x)>MAX_SPEED)
			b->vel.x = b->vel.x/std::abs(b->vel.x)*MAX_SPEED;

		b->vel.x -= (b->vel.x>0?1:(b->vel.x<0?-1:0))*FD::delta*5; //drag
		FizThing::update();
	}
};
