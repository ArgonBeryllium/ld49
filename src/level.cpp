#include "level.h"
#include "resources.h"
#include <shitrndr.h>

int Level::level_i = 0;
S_Level* S_Level::instance = 0;
S_Win* S_Win::instance = 0;

void rtc(v2i pos, std::string t, render::TextData td)
{
	static std::map<std::pair<std::string, render::TextData*>, std::pair<SDL_Texture*, v2i>> cache;
	std::pair<std::string, render::TextData*> p = {t, &td};
	if(cache[p].first)
	{
		v2f siz = cache[p].second.to<float>()*td.scale;
		pos -= siz*td.anchor;

		SDL_Rect r = {pos.x, pos.y, int(siz.x), int(siz.y)};
		SDL_Point c = {int(siz.x*td.anchor.x), int(siz.y*td.anchor.y)};
		SDL_RenderCopyEx(ren, cache[p].first, 0, &r, td.angle*180./M_PIf32, &c, SDL_FLIP_NONE);
		return;
	}
	td.render = 1;
	td.destroy = 0;
	SDL_Texture* tex = render::text(pos, t, td);
	v2i s;
	SDL_QueryTexture(tex, 0, 0, &s.x, &s.y);
	cache[p] = {tex, s};
}

static std::vector<std::string> split(const std::string& s, const char& c)
{
	std::vector<std::string> out;
	std::string cur;
	for(char e : s)
	{
		if(e==c && !cur.empty())
		{
			out.push_back(cur);
			cur = {};
			continue;
		}
		cur += e;
	}
	if(!cur.empty()) out.push_back(cur);
	return out;
}

S_TextCrawl::S_TextCrawl(const std::string& text, const uint8_t& sceneToLoad) : lines(split(text, '#')), stl(sceneToLoad)
{
	to_dur = 0;
}

void S_TextCrawl::load() { shitrndr::bg_col = {0,0,0,255}; }

void S_TextCrawl::loop()
{
	if(line==lines.size()) return;
	float d = FD::delta;
	if(curP<lines[line].length())
	{
		curP += d*35+5*Input::getKey(SDLK_LCTRL);
		if((int)(curP)>(int)(curP-d*15+5*Input::getKey(SDLK_LCTRL)) && lines[line][curP] && lines[line][curP] != ' ' && lines[line][curP] != '\n' && lines[line][curP] != ',' && lines[line][curP] != '.')
			audio::play(S_TEXT_BLEEP, .2);
	}
	std::string l;
	for(uint8_t i = 0; i < curP && i < lines[line].length(); i++)
		l += lines[line][i];

	int yo = 0;
	for(std::string subline : split(l, '\n'))
	{
		if(!subline.empty())
			render::text({WindowProps::getWidth()/2, WindowProps::getHeight()/2+yo}, subline.c_str(), TD_DEF_C);
		yo += 10;
	}

	if(curP>=lines[line].length())
	{
		if(!Input::getKey(SDLK_RETURN) &&!Input::getKey(SDLK_SPACE) && !Input::getKey(SDLK_LCTRL)) return;
		curP = 0;
		line++;
		if(line==lines.size()) setActive(stl);
		std::cout << line << ", " << lines.size() << '\n';
	}
}
