#pragma once
#include "objects.h"

struct Platform : FizThing
{
	static Platform* instance;
	float av = 0;

	Platform() : FizThing({0, 2}, {18, 2}, 0)
	{
		instance = this;
		b->onCollision = [](CollisionData cd, Body* self, Body* o)
		{
			Platform* ins = Platform::instance;
			if(dynamic_cast<CircleCollider*>(o->col)) std::cout << "CUM\n";
			ins->applyForce(o->tr.pos.x, o->vel.getLength()*v2f::dot(o->vel.normalised(), ins->getUp()*-1)*o->mass);
		};
	}

	inline std::pair<float, float> getLimitsH()
	{
		return std::make_pair(b->tr.transformPoint(((PolygonCollider*)b->col)->verticies[0]).x, b->tr.transformPoint(((PolygonCollider*)b->col)->verticies[1]).x);
	}
	inline bool withinH(const Body* o)
	{
		auto lims = getLimitsH();
		return o->tr.pos.x > lims.first && o->tr.pos.x < lims.second;
	}
	inline bool withinV(const Body* o, float epsilon = .01)
	{
		b->tr.scl.y += epsilon;
		CollisionData cd = b->col->testCollision(o->col);
		b->tr.scl.y -= epsilon;
		return cd.colliding && v2f::dot(cd.normal, v2f(0,1))>0;
	}
	inline bool onPlatform(const Body* o, float epsilon = .01) { return withinH(o) && withinV(o, epsilon); }

	float getBalancePointX()
	{
		return b->tr.pos.x+b->tr.rot*b->tr.scl.x*getRight().x;
	}

	void applyForce(float x, float mag)
	{
		constexpr float RESISTANCE = .02;
		av += -x*mag*RESISTANCE;
	}

	void update() override
	{
		float wd = 0;
		float ms = 0;
		/*
		// sum forces & masses
		for(Body* o : b->world->bodies)
		{
			if(o==b || o->passive || !onPlatform(o)) continue;

			wd += o->vel.getLength() * std::max(0.f, v2f::dot(o->vel, getUp()*-1))
					* o->mass
					* (o->tr.pos.x/getLimitsH().second);
			ms += o->mass;
		}
		// apply forces to av if there are any
		if(wd != 0)
		{
			wd /= b->world->bodies.size();
			av += ms*-wd*FD::delta*.3;
		}
		*/

		// update rotation
		b->tr.rot += av*FD::delta;

		// limit angles & bounce on extremes
		constexpr float ANGLE_LIMIT = M_PIf32*.3;
		if(b->tr.rot>ANGLE_LIMIT)
		{
			b->tr.rot = ANGLE_LIMIT;
			av = -std::abs(av)*.5;
		}
		else if(b->tr.rot<-ANGLE_LIMIT)
		{
			b->tr.rot = -ANGLE_LIMIT;
			av = std::abs(av)*.5;
		}

		// friction
		av *= 1-FD::delta*2;
	}

	void render() override
	{
		SDL_Rect r = getRect();
		r.x -= r.w/2;
		r.y -= r.h/2;
		SDL_RenderCopyEx(shitrndr::ren, T_PLATFORM, 0, &r, -b->tr.rot*180/M_PIf32, 0, SDL_FLIP_NONE);
		FizThing::render();
	}
};

