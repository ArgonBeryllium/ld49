#include "objects.h"
#include "platform.h"
#include "player.h"
#include "level.h"
using namespace shitrndr;

void FizThing::update()
{
	b->vel.y = std::min(b->vel.y, MAX_VEL_Y);
	if(Platform::instance->onPlatform(b))
	{
		v2f pu = Platform::instance->getUp();
		b->tr.rot = std::atan2(-pu.y, pu.x)-M_PI_2f32;
	}
	pos = b->tr.pos;
	pos.y *= -1;

	if(pos.y>26) active = false;
}
void FizThing::render()
{
	SetColour({255,0,0,255});
	std::vector<v2f>& vs = ((PolygonCollider*)b->col)->verticies;
	for(int i = 0; i != vs.size(); i++)
		render::line(b->tr.transformPoint(vs[i])*v2f(1,-1), b->tr.transformPoint(vs[(i+1)%vs.size()])*v2f(1,-1));
}

void Player::die()
{
	b->tr.pos = {0, 4};
	S_Level::instance->scheduleReload();
	audio::play(S_DIE);
}

World* FizThing::wld = 0;
Platform* Platform::instance = 0;
Player* Player::instance = 0;
std::map<Body*, FizThing*> FizThing::lookup = {};
Boss* Boss::inst = 0;
