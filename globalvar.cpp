#include "globalvar.h"

#include <prism/blitz.h>

static struct {
	map<int, int> mVars;

} gGlobalVar;

static void loadGlobalVarHandler(void* tData) {
	gGlobalVar.mVars.clear();
}

ActorBlueprint getGlobalVarHandler()
{
	return makeActorBlueprint(loadGlobalVarHandler);
}

void setGlobalVar(int tIndex, int tValue)
{
	gGlobalVar.mVars[tIndex] = tValue;
}

int getGlobalVar(int tIndex)
{
	return gGlobalVar.mVars[tIndex];
}
