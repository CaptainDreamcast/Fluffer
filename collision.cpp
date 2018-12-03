#include "collision.h"

#include <prism/blitz.h>

static struct {
	int mPlayerList;
	int mNonPlayerList;

} gData;

void loadGameCollisions() {
	gData.mPlayerList = addCollisionListToHandler();
	gData.mNonPlayerList = addCollisionListToHandler();
}

int getPlayerCollisionList()
{
	return gData.mPlayerList;
}

int getNonPlayerCollisionList()
{
	return gData.mNonPlayerList;
}
