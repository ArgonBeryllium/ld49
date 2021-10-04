#include <cumt/cumt.h>
#include <fizzyx/fizzyx_solvers.h>
#include <shitrndr.h>
#include <fizzyx/fizzyx.h>
#include <type_traits>

#include "resources.h"
#include "states.h"
#include "expirables.h"
#include "objects.h"
#include "platform.h"
#include "player.h"
#include "hazard.h"
#include "enemies.h"
#include "level.h"

using namespace cumt;

void gameStart();
void gameLoop();
void gameKeyDown(const SDL_Keycode& key);
void renderSplash();

int CUMT_MULP_MAIN()
{
	InitParams ip;
	ip.sr_ps = 2;

	quickInit(960, 652, ip);
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
		if(nactive!=1) State::setActive(1);
	}
};


void gameStart()
{
	WindowProps::setLockType(WindowProps::BARS);
	SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

	loadResources();

	//State::states = {new S_Splash(), new S_Level()};
	State::states = {new S_Splash(), new S_TextCrawl("Deep in the mountains lays the origin of many a legend.#The local peaks have been said to house\nwhole pantheons of various spirits\nand fantasy creatures.#But one peak stands apart from its neighbours -#The Mountain of Might.#After a trecherous hike,\ntrials undreamt of by most people,\nawaits a final test.#...#Hey, can we just agree to\npretend I wrote a complete, moody intro?\nI don't really have time to spare\nat the moment.\nThanks!#Anyway, TL;DR buff guys go up the mountain\nto fight for honour or something.", 2), new S_Menu(), new S_Level(), new S_Loader(), new S_Win(), new S_Credits(), new S_Controls()};
	State::allStart();
}

void gameLoop()
{
	State::getActive()->loop();
	State::update();
	//common::renderFPS({});
	//render::text({5,6}, std::to_string(State::getActive()->index), TD_DEF_L);
	//render::text({5,12}, std::to_string(Player::instance->parent_set->things_id.size()), TD_DEF_L);
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
