#pragma once
#include "player.h"
#include "platform.h"
#include "hazard.h"

struct Minion : Fighter
{
	SDL_Texture* tex = T_P_IDLE;
	float dir = -1;
	Minion(v2f pos_, v2f scl_ = {1,1}, float mass_ = 1) : Fighter(pos_, scl_, mass_)
	{
		acc = 15;
		health = health_max = .5;
		a_strength = .1;
		a_cooldown = .5;
	}
	void move() override
	{
		v2f tv = Player::instance->pos-pos;
		if(tv.getLengthSquare()<8)
		{
			if(std::rand()%20==0)
				attack(tv.normalised());
			dir = tv.x/std::abs(tv.x);
		}
		else
		{
			auto l = Platform::instance->getLimitsH();
			float dtc = std::max(l.first+1, std::min(l.second-1, Platform::instance->getBalancePointX()))-pos.x;
			dir = dtc/std::abs(dtc);
			if(std::abs(pos.x-Platform::instance->getBalancePointX())>2)
				jump();
		}
		float ximp = acc*FD::delta*(dir+(common::frand()*2-1)*.1);
		if(Platform::instance->onPlatform(b))
		{
			b->vel += ximp*getRight();
		}
		else
			b->vel.x += ximp*.5;
	}
	void drawHP()
	{
		if(health<health_max)
		{
			SDL_Rect r = getRect();
			SetColour(C_WHITE);
			FillRect({r.x+(dir<0?r.w/2:0), r.y-5, int(r.w*health/health_max), 3});
			DrawRect({r.x+(dir<0?r.w/2:0), r.y-5, r.w, 3});
		}
	}
	void drawBG()
	{
		SDL_Rect r = getRect();
		SetColour(C_BG);
		v2i pp = Thing2D::spaceToScr(pos);
		FillCircle(pp.x+(dir<0?Thing2D::getScalar()/2:0), pp.y, std::sin(FD::time)*2+Thing2D::getScalar()/2);
	}
	void render() override
	{
		drawBG();
		if(t_ac > a_cooldown/2) tex = T_P_PUNCH;
		else if(!Platform::instance->onPlatform(b)) tex = T_P_AIRBORNE;
		else if(common::inVec().x) tex = T_P_RUN[int(FD::time*4)%2];
		else tex = T_P_IDLE;
		SDL_Rect r = getRect();
		r.y -= r.h/2;
		r.x -= r.w/2;
		r.w *= 1.5;
		RenderCopyEx(ren, tex, {0,0,48,32}, r, -b->tr.rot*180/M_PIf32+180, {16,16}, dir>0?SDL_FLIP_NONE:SDL_FLIP_HORIZONTAL);
		drawHP();
	}
};
struct Brute : Minion
{
	Brute(v2f pos_) : Minion(pos_, {2,2}, 3)
	{
		acc = 7;
		jump_h = 4;
		a_strength = .4;
		a_cooldown = .8;
		health = health_max = 3;
		b->bitmask = 0b00000100;
	}
	void render() override
	{
		drawBG();
		if(t_ac > a_cooldown/2) tex = T_B_PUNCH;
		else if(!Platform::instance->onPlatform(b)) tex = T_B_AIRBORNE;
		else if(common::inVec().x) tex = T_B_RUN[int(FD::time*4)%2];
		else tex = T_B_IDLE;
		SDL_Rect r = getRect();
		r.y -= r.h/2;
		r.x -= r.w/2;
		r.w *= 1.5;
		RenderCopyEx(ren, tex, {0,0,96,64}, r, -b->tr.rot*180/M_PIf32+180, {32,32}, dir>0?SDL_FLIP_NONE:SDL_FLIP_HORIZONTAL);
		drawHP();
	}
};

struct Boss : Brute
{
	static Boss* inst;
	int as = 0;
	struct BossState
	{
		float st;
		virtual void update() {}
	};
	struct BS_Def : BossState
	{
		void update() override
		{
			Boss::inst->Brute::update();
			float t = FD::time-st;
			constexpr float tl = 10;
			auto* b = Boss::inst;
			if(b->t_ac > b->a_cooldown/2) b->tex = T_B_PUNCH;
			else if(!Platform::instance->onPlatform(b->b)) b->tex = T_B_AIRBORNE;
			else if(common::inVec().x) b->tex = T_B_RUN[int(FD::time*4)%2];
			else b->tex = T_B_IDLE;

			if(t>tl-1)
			{
				t -= tl-1;
				b->b->vel *= 0;
				b->b->tr.pos = common::lerp(b->b->tr.pos, {0,20}, t);
			}
			if(FD::time-st > tl)
			{
				b->b->vel *= 0;
				b->b->tr.pos = {0,20};
				Boss::inst->setActive(1);
			}
		}
	};
	struct BS_Wee : BossState
	{
		void update() override
		{
			static bool ff = 1;
			static v2f op, os;

			float t = FD::time-st;
			auto* b = Boss::inst;

			if(ff)
			{
				ff = 0;
				op = {0,5};
				os = b->scl;
				wld->removeBody(b->b);
				return;
			}
			if(t<1)
			{
				b->pos = common::lerp(b->pos, {0,.5}, t);
				b->scl = common::lerp(b->scl, {1,1}, t);
			}
			else
			{
				t -= 1;
				if(std::rand()%50==0)
					b->parent_set->instantiate(new Hazard((common::frand()*2-1)*Platform::instance->scl.x));
			}
			if(t>4 && t<5)
			{
				t -= 4;
				b->pos = common::lerp(b->pos, op*v2f(1,-1), t);
				b->scl = common::lerp(b->scl, os, t);
				return;
			}
			if(t >= 5)
			{
				Boss::inst->setActive(0);
				b->b->tr.scl = os;
				b->b->tr.pos = op;
				b->b->vel *= 0;
				wld->addBody(b->b);
				ff = 1;
			}
		}
	};
	BossState* states[2]{new BS_Def(), new BS_Wee()};

	void setActive(int i)
	{
		as = i;
		states[as]->st = FD::time;
	}

	Boss(v2f pos_) : Brute(pos_)
	{
		inst = this;
		health = health_max = 4;
		acc = 10;
		setActive(0);
	}
	~Boss()
	{
		for(int i = 0; i != 3; i++) delete states[i];
	}
	void update() override
	{
		states[as]->update();
	}
	void render() override
	{
		drawBG();
		SDL_Rect r = getRect();
		r.y -= r.h/2;
		r.x -= r.w/2;
		r.w *= 1.5;
		RenderCopyEx(ren, tex, {0,0,96,64}, r, -b->tr.rot*180/M_PIf32+180, {32,32}, dir>0?SDL_FLIP_NONE:SDL_FLIP_HORIZONTAL);
		drawHP();
	}
};
