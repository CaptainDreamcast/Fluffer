#include "helplist.h"


static void loadHelpList(void* tData) {


}

static void unloadHelpList(void* tData) {


}

ActorBlueprint getHelpList()
{
	return makeActorBlueprint(loadHelpList, unloadHelpList);
}
