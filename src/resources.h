#pragma once
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_render.h>
#include <cumt/cumt.h>
#include <SDL2/SDL_image.h>
#include <cumt/cumt_render.h>

static inline SDL_Colour hexc(int c) { return {Uint8(c>>16), Uint8((c>>8) & 0xFF), Uint8(c & 0xFF),255}; }

inline SDL_Texture* SHEET_LOGO,
	   *T_PLATFORM, *T_P_IDLE, **T_P_RUN, *T_P_PUNCH, *T_P_CROUCH, *T_P_AIRBORNE, *T_P_FALL,
	   *T_B_IDLE, **T_B_RUN, *T_B_PUNCH, *T_B_AIRBORNE,
	   *T_BG, *T_MT, *T_HAZARD;
inline SDL_Colour C_BG = hexc(0xff7777), C_WHITE = hexc(0xffffff);
inline Mix_Chunk* S_SPLASH, *S_TEXT_BLEEP;

inline cumt::render::TextData TD_DEF_L, TD_DEF_C, TD_DEF_R, TD_JP_C;

inline SDL_Texture* loadTexture(const char* path)
{
	SDL_Texture* out = IMG_LoadTexture(shitrndr::ren, (std::string("res/")+path).c_str());
	if(!out) std::cerr << "couldn't load texture: " << IMG_GetError() << '\n';
	return out;
}
inline Mix_Chunk* loadSound(const char* path)
{
	Mix_Chunk* out = Mix_LoadWAV((std::string("res/")+path).c_str());
	if(!out) std::cerr << "couldn't load chunk: " << Mix_GetError() << '\n';
	return out;
}
inline void loadResources()
{
	using namespace cumt;
	audio::init();

	SHEET_LOGO = loadTexture("logo.png");
	S_SPLASH = loadSound("splash.wav");

	S_TEXT_BLEEP = loadSound("bleep.wav");

	T_PLATFORM   = loadTexture("platform.png");

	T_BG = loadTexture("bg.png");
	T_MT = loadTexture("mt.png");
	T_HAZARD = loadTexture("hazard.png");

	T_P_IDLE     = loadTexture("player1.png");
	T_P_RUN      = new SDL_Texture*[2]{loadTexture("player2.png"), loadTexture("player3.png")};
	T_P_PUNCH     = loadTexture("player4.png");
	T_P_CROUCH   = loadTexture("player5.png");
	T_P_AIRBORNE = loadTexture("player6.png");
	T_P_FALL     = loadTexture("player7.png");

	T_B_IDLE     = loadTexture("brute1.png");
	T_B_RUN      = new SDL_Texture*[2]{loadTexture("brute2.png"), loadTexture("brute3.png")};
	T_B_PUNCH     = loadTexture("brute4.png");
	T_B_AIRBORNE = loadTexture("brute5.png");

	RenderData::loadFont("res/ProggyTiny.ttf", 14);
	RenderData::loadFont("res/nicomoji-plus_1.11.ttf", 12);
	RenderData::loadFont("res/m6x11.ttf", 14);
	TD_DEF_L = render::TextData{0, render::TextData::LEFT  };//, {1,1}, 0, {0,0,0,255}};
	TD_DEF_C = render::TextData{0, render::TextData::CENTRE};//, {1,1}, 0, {0,0,0,255}};
	TD_DEF_R = render::TextData{0, render::TextData::RIGHT };//, {1,1}, 0, {0,0,0,255}};
	TD_JP_C = render::TextData{1, render::TextData::CENTRE};
}
