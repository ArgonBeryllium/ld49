#pragma once
#include "player.h"
#include "platform.h"
#include "states.h"
#include "enemies.h"
#include "hazard.h"
#include <SDL2/SDL_keycode.h>
#include <shitrndr.h>

struct Wave
{
	bool started = false;
	std::vector<FizThing*> things;
	Wave(std::vector<FizThing*> things_) : things(things_)
	{
		for(auto t : things)
			t->wld->removeBody(t->b);
	}
	bool done()
	{
		for(auto t : things)
			if(t->active) return false;
		return true;
	}
	void clear()
	{
		for(auto t : things)
			if(t)
			{
				if(t->parent_set) t->parent_set->destroy(t);
				else delete t;
			}
	}
	void start()
	{
		for(auto t : things)
		{
			t->wld->addBody(t->b);
			Player::instance->parent_set->instantiate(t);
		}
		started = true;
	}
};
struct Level
{
	static void (*restart)();
	std::vector<Wave> waves;
	size_t i = 0;
	static int level_i;

	Level(std::vector<Wave> waves_) : waves(waves_) {}

	void win()
	{
		std::cout << "bing bong wahoo.\n";
		State::setActive(State::active+1);
		level_i++;
	}
	void clear()
	{
		for(Wave w : waves) w.clear();
	}
	void update()
	{
		if(i==waves.size()) return;
		if(waves[i].done())
		{
			std::cout << "wave " << std::to_string(i) << " clear\n";
			i++;
			if(i==waves.size()) win();
			else waves[i].start();
		}
		else if(!waves[0].started) waves[0].start();
	}
};

struct S_Level : State
{
	static S_Level* instance;
	ThingSet set;
	World w;
	Player* pr;
	Platform* pl;
	Level level = {{}};
	float fadein_alpha = 0;

	void start() override
	{
		FizThing::wld = &w;
		w.ec->addEffector(new Eff_Gravity());
		ti_dur = .5;
		to_dur = .5;
	}

	bool reloading = false;
	void scheduleReload() { reloading = true; }
	void reload()
	{
		level.clear();
		set.clear();
		load();
		fadein_alpha = 1;
		reloading = false;
	}
	void load() override
	{
		instance = this;
		bg_col = {C_BG};
		Thing2D::view_scale = .6;
		std::srand(FD::time);

		pr = set.instantiate(new Player());
		pl = set.instantiate(new Platform());

		switch(Level::level_i)
		{
			case 0:
				level = Level({Wave({new Minion({4, -20})}),
						Wave({new Hazard(0)}),
						Wave({new Minion({-4, -20}), new Minion({4, -10})})});
				break;
		}
	}
	void loop() override
	{
		if(!pr->active)
			pr->die();
		if(reloading)
			reload();
		w.step(FD::delta);
		set.update();
		level.update();

		shakeUpdate();
		v2i vp = Thing2D::spaceToScr({});
		Copy(T_BG, {vp.x-WindowProps::getWidth()/2, vp.y+128-WindowProps::getHeight()/2, WindowProps::getWidth(), WindowProps::getHeight()});
		set.render();
		SetColour(C_BG);
		v2i pp = Thing2D::spaceToScr(pr->pos);
		FillCircle(pp.x+(pr->dir<0?Thing2D::getScalar()/2:0), pp.y, std::sin(FD::time*4)*2+Thing2D::getScalar()/2);
		pr->render();

		if(fadein_alpha>0)
		{
			fadein_alpha -= FD::delta*2;
			SetColour({0,0,0,255});
			FillRect({0,0,WindowProps::getWidth(), int(WindowProps::getHeight()*fadein_alpha)});
		}
	}
	void onKey(SDL_Keycode key) override
	{
		switch(key)
		{
			case SDLK_z:
				pr->jump();
				break;
			case SDLK_x:
				pr->attack(pr->dir*pr->getRight());
				break;
			case SDLK_c:
				pr->flop();
				break;
			case SDLK_r:
				reload();
				break;
			case SDLK_BACKSPACE:
				level.win();
				break;
		}
	}
};

struct S_Win : State
{
	static S_Win* instance;
	S_Win()
	{
		instance = this;
	}
	void loop() override
	{
		SDL_RenderCopy(ren, T_BG, 0, 0);
		render::text({WindowProps::getWidth()/2, WindowProps::getHeight()/4}, "You Won", TD_DEF_C);
		render::text({WindowProps::getWidth()/2, WindowProps::getHeight()/3}, "Thanks for playing", TD_DEF_C);
		render::text({WindowProps::getWidth()/2, WindowProps::getHeight()/2}, "Made by ArBe for Ludum Dare #49", TD_DEF_C);
		render::text({WindowProps::getWidth()/2, WindowProps::getHeight()/3*2}, "Press [Q] to quit", TD_DEF_C);
	}
};
struct S_Loader : State
{
	float st;
	void load() override
	{
		st = FD::time;
	}
	void loop() override
	{
		render::text({WindowProps::getWidth()/2, WindowProps::getHeight()/2}, "Level "+std::to_string(Level::level_i+1), TD_DEF_C);
		if(FD::time-st>3)
			render::text({WindowProps::getWidth()/2, WindowProps::getHeight()/2+10}, "Press [Return] to continue", TD_DEF_C);
	}
	void onKey(SDL_Keycode key) override
	{
		if(key!=SDLK_RETURN) return;
		//if(S_Level::instance->index<1)
		//	setActive(S_Level::instance->index);
		//else
			setActive(S_Win::instance->index);
	}
};
struct S_TextCrawl : State
{
	std::vector<std::string> lines;
	size_t stl;
	float curP = 0;
	size_t line = 0;

	S_TextCrawl(const std::string& text_, const uint8_t& sceneToLoad);
	
	void load() override;
	void loop() override;
};
