#include "objects.h"
#include "platform.h"
#include "player.h"
using namespace shitrndr;

void FizThing::update()
{
	b->vel.y = std::min(b->vel.y, MAX_VEL_Y);
	if(Platform::instance->onPlatform(b))
	{
		v2f pu = Platform::instance->getUp();
		b->tr.rot = std::atan2(pu.y, pu.x)+M_PI_2f32;
		b->tr.pos += getUp()*FD::delta;
	}
	pos = b->tr.pos;
	pos.y *= -1;
}
void FizThing::render()
{
	SetColour({255,0,0,255});
	std::vector<v2f>& vs = ((PolygonCollider*)b->col)->verticies;
	for(int i = 0; i != vs.size(); i++)
		render::line(b->tr.transformPoint(vs[i])*v2f(1,-1), b->tr.transformPoint(vs[(i+1)%vs.size()])*v2f(1,-1));
}

World* FizThing::wld = 0;
Platform* Platform::instance = 0;
Player* Player::instance = 0;
std::map<Body*, FizThing*> FizThing::lookup = {};
