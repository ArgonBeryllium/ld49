#include "objects.h"
using namespace shitrndr;

void FizThing::update()
{
	CollisionData cd = b->col->testCollision(Platform::instance->b->col);
	if(cd.colliding)
	{
		b->tr.rot = std::atan2(cd.normal.y, cd.normal.x)+M_PI_2f32;
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
