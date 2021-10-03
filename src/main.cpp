#include <cumt/cumt.h>
#include <fizzyx/fizzyx_solvers.h>
#include <shitrndr.h>
#include <fizzyx/fizzyx.h>

#include "resources.h"
#include "states.h"
#include "expirables.h"
#include "objects.h"
#include "platform.h"
#include "player.h"
#include "hazard.h"

using namespace cumt;

void gameStart();
void gameLoop();
void gameKeyDown(const SDL_Keycode& key);
void renderSplash();

int CUMT_MULP_MAIN()
{
	InitParams ip;
	ip.sr_ps = 2;

	quickInit(960, 720, ip);
	onLoop = &gameLoop;
	onKey = &gameKeyDown;
	gameStart();
	shitrndr::onKeyUp = [](const SDL_Keycode& key) { State::getActive()->onKeyUp(key); };
	shitrndr::onKeyHeld = [](const SDL_Keycode& key) { State::getActive()->onKeyHeld(key); };
	shitrndr::onMBDown = [](const uint8_t& b) { State::getActive()->onMB(b); };
	shitrndr::onMBUp = [](const uint8_t& b) { State::getActive()->onMBUp(b); };
	start();
}

using namespace shitrndr;
using namespace fizzyx;

struct S_Splash : State
{
	float del = 2;
	void transOut(float t) override
	{
		SetColour({0,0,0,Uint8(std::min(1.f,t)*255)});
		SDL_RenderFillRect(ren, 0);
	}
	void transIn(float t) override { transOut(1-t); }

	void loop() override
	{
		static bool played = 0;
		if(FD::time>2 && !played)
		{
			audio::play(S_SPLASH, .3);
			played = 1;
		}

		bg_col = {0,0,0,255};
		v2i p = WindowProps::getSize()/2;
		int s = 100;
		int f = std::max(0,std::min(int((FD::time-del)*30), 28));
		Copy(SHEET_LOGO, {540*f,0,540,540}, {p.x-s/2, p.y-s/2, s, s});

		render::text(p+v2i{0,s/2}, "a game by arbe", TD_DEF_C);
		//render::text(p+v2i{0,s/2+12}, "アルビのゲーム", TD_JP_C);

		if(nactive==active && FD::time>4) setActive(1);
	}
	void onKey(SDL_Keycode key) override
	{
		State::setActive(1);
	}
};

struct S_Test : State
{
	ThingSet set;
	World w;
	Player* pr;
	Platform* pl;
	void start() override
	{
		FizThing::wld = &w;
		w.ec->addEffector(new Eff_Gravity());
		pr = set.instantiate(new Player());
		pl = set.instantiate(new Platform());
		std::srand(FD::time);
		set.instantiate(new Fighter({common::frand()*12-6, -3}));
	}
	void load() override
	{
		bg_col = {255,255,255,255};
		Thing2D::view_scale = .2;
	}
	void loop() override
	{
		static float scd = 2;
		if(scd<0)
		{
			scd = common::frand()+1;
			float m = .5+common::frand()*1.5;
			//set.instantiate(new Fighter({common::frand()*12-6, -10}, {m,m}, m));
		}
		scd -= FD::delta;

		static float hcd = 2;
		if(hcd<0)
		{
			hcd = common::frand()+2;
			set.instantiate(new Hazard(common::frand()*12-6));
		}
		hcd -= FD::delta;

		w.step(FD::delta);
		set.update();
		set.render();
		SetColour({0,0,0,255});
		v2i pp = Thing2D::spaceToScr({Platform::instance->getBalancePointX(), 0});
		FillCircle(pp.x, pp.y, 3);
		pp = Thing2D::spaceToScr({Platform::instance->getLimitsH().first, 0});
		FillCircle(pp.x, pp.y, 4);
		pp = Thing2D::spaceToScr({Platform::instance->getLimitsH().second, 0});
		FillCircle(pp.x, pp.y, 4);
	}
	void onKey(SDL_Keycode key) override
	{
		switch(key)
		{
			case SDLK_z:
				pr->jump();
				break;
			case SDLK_x:
				pr->attack(v2f(pr->dir, 0));
				break;
			case SDLK_c:
				pr->flop();
				break;
		}
	}
};

void gameStart()
{
	WindowProps::setLockType(WindowProps::BARS);
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

	loadResources();

	//State::states = {new S_Splash(), new S_Test()};
	State::states = {new S_Test()};
	State::allStart();
}

void gameLoop()
{
	State::getActive()->loop();
	State::update();
	SetColour({0,0,0,255});
	FillRect({0,0,10,11});
	common::renderFPS({});
	render::text({5,6}, std::to_string(State::getActive()->index), TD_DEF_L);
}
void gameKeyDown(const SDL_Keycode& key)
{
	switch(key)
	{
		case SDLK_q :
			std::exit(0);
			break;
	}
	State::getActive()->onKey(key);
}
