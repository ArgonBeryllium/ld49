#pragma once
#include "expirables.h"
#include "resources.h"
#include "platform.h"
#include <shitrndr.h>
using namespace cumt;
using namespace shitrndr;

inline static float pamt, pdur, tl = 0;
inline void shakeCam(float amt = .5, float dur = .5)
{
	if(tl>0 && amt<pamt) return;
	pamt = amt;
	pdur = dur;
	tl = dur;
}
inline void shakeUpdate()
{
	tl -= FD::delta;
	if(tl<0)
	{
		pamt = pdur = tl = 0;
		Thing2D::view_pos = {};
		return;
	}
	Thing2D::view_pos = v2f(common::frand()*2-1,common::frand()*2-1).normalised()*pamt*tl/pdur;
}

struct PunchFX : Particles2D
{
	float st;
	v2f vels[20] = {};
	PunchFX(v2f pos_, float str, v2f scl_ = {1,1}) : Particles2D(20*str), st(FD::time)
	{
		for(int i = 0; i != count; i++)
		{
			poss[i] = pos_ + scl*v2f(common::frand()-.5, common::frand()-.5);
			lives[i] = .2 + common::frand();
			vels[i] = v2f(common::frand()*2-1,common::frand()*2-1).normalised()*(common::frand()+.5)*5*str;
		}
	}
	void update() override
	{
		if(FD::time-st>1.2) parent_set->scheduleDestroy(this);
	}
	void renderParticle(size_t i) override
	{
		if(lives[i]<=0) return;
		lives[i] -= FD::delta*2;
		poss[i] += vels[i]*FD::delta;
		vels[i].y += FD::delta*3;
		SetColour(C_WHITE);
		v2i pp = spaceToScr(poss[i]);
		FillCircle(pp.x, pp.y, 8*lives[i]);
	}
};
