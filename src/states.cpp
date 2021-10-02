#include <cumt/cumt.h>
#include "states.h"

using namespace cumt;

size_t State::next_index = 0;
std::vector<State*> State::states = {};
size_t State::active = 0;
size_t State::nactive = 0;

static float tt = 0;
static bool t = 1;

State* State::getActive() { return states[active]; }
void State::setActive(size_t i)
{
	nactive = i;
	t = 1;
	tt = 0;
}
void State::allStart()
{
	for(State* s : states)
		s->start();
	getActive()->load();
}
void State::update()
{
	if(active==nactive)
	{
		if(t)
		{
			tt += FD::delta/getActive()->ti_dur;
			getActive()->transIn(tt);
			if(tt>=1)t = 0;
		}
	}
	else
	{
		tt += FD::delta/getActive()->to_dur;
		getActive()->transOut(tt);
		if(tt>=1)
		{
			getActive()->unload();
			active = nactive;
			getActive()->load();
			tt = 0;
		}
	}
}
