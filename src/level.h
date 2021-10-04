#pragma once
#include "player.h"
#include "platform.h"
#include "states.h"
#include "enemies.h"
#include "hazard.h"
#include <SDL2/SDL_keycode.h>
#include <shitrndr.h>

void rtc(v2i pos, std::string t, render::TextData td = TD_DEF_C);

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
		Thing2D::view_scale = .6;
		std::srand(FD::time);

		pr = set.instantiate(new Player());
		pl = set.instantiate(new Platform());

		switch(Level::level_i)
		{
			case 0:
				level = Level({Wave({new Minion({4, -20})}),
						Wave({new Hazard(0)}),
						Wave({new Minion({-4, -10}), new Minion({4, -10})})});
				break;
			case 1:
				level = Level({Wave({new Brute({4, -20})}),
						Wave({new Hazard(-4), new Hazard(4)}),
						Wave({new Minion({-4, -20}), new Minion({4, -10}), new Brute({0, -15})})});
				break;
			case 2:
				level = Level({Wave({new Minion({-4, -20}), new Minion({4, -10}), new Brute({-3, -15}), new Brute({3, -15})}),
						Wave({new Minion({-8, -10}), new Minion({-6,-15}), new Minion({-4,-20}), new Minion({0,-35})})});
				break;
			case 3:
				level = Level({Wave({new Hazard(-5), new Hazard(5)}),
						Wave({new Hazard(-3), new Hazard(3)}),
						Wave({new Hazard(0)}),
						Wave({new Boss({0, -10})}),
						Wave({new Hazard(0)})});

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
		SetColour(C_BG);
		SDL_RenderFillRect(ren, 0);
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
		Copy(T_BG, WindowProps::getSizeRect());
		render::text({WindowProps::getWidth()/2, WindowProps::getHeight()/3}, "Thanks for playing", TD_DEF_C);
		render::text({WindowProps::getWidth()/2, WindowProps::getHeight()/2}, "Made by ArBe for Ludum Dare #49", TD_DEF_C);
		render::text({WindowProps::getWidth()/2, WindowProps::getHeight()/5*4}, "Press [Q] to quit", TD_HL);
	}
};
struct S_Loader : State
{
	float st;
	int max = 4;
	void load() override
	{
		st = FD::time;
		bg_col = C_BLACK;
	}
	void loop() override
	{
		render::text({WindowProps::getWidth()/2, WindowProps::getHeight()/2}, Level::level_i==max?"You Won":"Level "+std::to_string(Level::level_i+1), TD_HL);
		if(FD::time-st>3)
			render::text({WindowProps::getWidth()/2, WindowProps::getHeight()/2+20}, "Press [Return] to continue", TD_DEF_C);
	}
	void onKey(SDL_Keycode key) override
	{
		if(key!=SDLK_RETURN) return;
		if(Level::level_i!=max)
		{
			S_Level::instance->scheduleReload();
			setActive(S_Level::instance->index);
		}
		else
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

struct S_Menu : State
{
	struct menuItem
	{
		const char* name;
		void(*onClicked)();
	};

	std::vector<menuItem> items;
	int sel = 0;
	float so = 0, sot = 0;
	void start() override
	{
		items.push_back({"START", [](){State::setActive(3);}});
		items.push_back({"CONTROLS", [](){State::setActive(7);}});
		items.push_back({"CREDITS", [](){State::setActive(6);}});
		items.push_back({"QUIT", [](){SDL_Quit(); std::exit(0);}});
	}
	void loop() override
	{
		int W = WindowProps::getWidth();
		int H = WindowProps::getHeight();
		rtc({W/2, H/3}, "Mountain of Might", TD_HL);
		float c = std::sin(FD::time*.25)*std::sin(FD::time*.2);
		if(sot>0)
		{
			sot -= FD::delta;
			so = easings::easeInElastic(sot);
		}
		else so = 0;
		for(int i = 0; i != items.size(); i++)
		{
			std::string t = items[i].name;
			if(i==sel) t= "> "+t+" <";
			rtc({W/2+(i==sel?int(so*10):0), H/2+int(H/5/items.size()*i)}, t, TD_DEF_C);
		}
	}
	void onKey(SDL_Keycode key) override
	{
		switch(key)
		{
			case SDLK_RETURN:
				audio::play(S_BEEP);
				items[sel].onClicked();
				break;
			case SDLK_UP:
			case SDLK_w:
				audio::play(S_BOOP);
				sot = 1;
				sel--;
				while(sel<0)sel+=items.size();
				sel %= items.size();
				break;
			case SDLK_DOWN:
			case SDLK_s:
				audio::play(S_BOOP);
				sot = 1;
				sel++;
				sel %= items.size();
				break;
		}
	}
};
struct S_Credits : State
{
	void load() override
	{
		bg_col = C_BLACK;
	}
	void loop() override
	{
		int W = WindowProps::getWidth();
		int H = WindowProps::getHeight();
		rtc({W/2, H/4}, "CREDITS", TD_HL);
		rtc({W/2, int(H*.4)}, "This game was made", TD_DEF_C);
		rtc({W/2, int(H*.5)}, "within 48 hours", TD_DEF_C);
		rtc({W/2, int(H*.6)}, "by ArBe (@argon_beryllium)", TD_DEF_C);
		rtc({W/2, int(H*.7)}, "for Ludum Dare 49.", TD_DEF_C);
		rtc({W/2, int(H*.9)}, "> BACK <", TD_DEF_C);
	}
	void onKey(SDL_Keycode key) override
	{
		if(key==SDLK_RETURN)
		{
			audio::play(S_BEEP);
			State::setActive(2);
		}
	}
};
struct S_Controls : State
{
	void load() override
	{
		bg_col = C_BLACK;
	}
	void loop() override
	{
		int W = WindowProps::getWidth();
		int H = WindowProps::getHeight();
		rtc({W/2, H/4}, "CONTROLS", TD_HL);
		rtc({W/2, int(H*.4)}, "ARROW KEYS - move", TD_DEF_C);
		rtc({W/2, int(H*.5)}, "[Z] - jump", TD_DEF_C);
		rtc({W/2, int(H*.6)}, "[X] - punch", TD_DEF_C);
		rtc({W/2, int(H*.7)}, "[C] - drop (watch cooldown below HP bar)", TD_DEF_C);
		rtc({W/2, int(H*.8)}, "Health regenerates automatically, both for you and your enemies.", TD_DEF_C);
		rtc({W/2, int(H*.95)}, "> BACK <", TD_DEF_C);
	}
	void onKey(SDL_Keycode key) override
	{
		if(key==SDLK_RETURN)
		{
			audio::play(S_BEEP);
			State::setActive(2);
		}
	}
};
