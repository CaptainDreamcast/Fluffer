#include "adventurescreen.h"

#include <prism/blitz.h>

#include "player.h"
#include "stage.h"
#include "files.h"
#include "collision.h"
#include "enemyhandler.h"
#include "textboxhandler.h"
#include "globalvar.h"

static void loadAdventureScreen() {

	instantiateActor(getGlobalVarHandler());

	loadGlobalFiles();
	loadGameCollisions();

	instantiateActor(getStage());
	instantiateActor(getPlayer());
	instantiateActor(getTextboxHandler());
	instantiateActor(getEnemyHandler());

	addFadeIn(60, NULL, NULL);
	playTrack(3);
}


static Screen gAdventureScreen;

Screen * getAdventureScreen()
{
	gAdventureScreen = makeScreen(loadAdventureScreen);
	return &gAdventureScreen;
}
