#pragma once
#include <cumt/cumt_things.h>
#include <shitrndr.h>
#include <vector>

struct State
{
	static std::vector<State*> states;
	static size_t active, nactive;
	static void setActive(size_t i);
	static State* getActive();

	static void allStart();
	static void update();

	float ti_dur = 1, to_dur = 1;

	cumt::ThingSet set;

	virtual void transOut(float t)
	{
		using namespace shitrndr;
		SetColour(bg_col);
		FillRect({0,0,WindowProps::getWidth(), int(WindowProps::getHeight()*t)});
	}
	virtual void transIn(float t)
	{
		using namespace shitrndr;
		SetColour(bg_col);
		FillRect({0,0,WindowProps::getWidth(), int(WindowProps::getHeight()*(1-t))});
	}

	virtual void start() {}
	virtual void load() {}
	virtual void unload() {}

	virtual void loop()
	{
		set.update();
		set.render();
	}
	virtual void onKey(SDL_Keycode key) {}
	virtual void onKeyUp(SDL_Keycode key) {}
	virtual void onKeyHeld(SDL_Keycode key) {}
	virtual void onMB(size_t b) {}
	virtual void onMBUp(size_t b) {}
};
