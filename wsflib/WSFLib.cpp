#include "../WSFGlob.h"
#include "../WSFMod.h"
#include "../WGMPack.h"

CWSFMod *newWSFMod()
{
	return new CWSFMod();
}

void freeWSFMod( CWSFMod *p )
{
	delete p;
}

CWSFPack *newWSFPack()
{
	return new CWSFPack();
}

void freeWSFPack( CWSFPack *p )
{
	delete p;
}

