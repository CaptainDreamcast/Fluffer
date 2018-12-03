#include "titlescreen.h"

#include <prism/blitz.h>

#include "titlescreen.h"

static struct {
	MugenSpriteFile mSprites;

} gData;

static void loadTitleScreen() {
	gData.mSprites = loadMugenSpriteFileWithoutPalette("title/TITLE.sff");
	addMugenAnimation(createOneFrameMugenAnimationForSprite(2, 0), &gData.mSprites, makePosition(0, 0, 1));
	addFadeIn(30, NULL, NULL);
	playTrack(4);
}


static void gotoAdventureScreen(void* tCaller) {
	(void)tCaller;
	setNewScreen(getTitleScreen());
}

static void updateTitleScreen() {

	if (hasPressedAFlank() || hasPressedStartFlank()) {
		addFadeOut(30, gotoAdventureScreen, NULL);
	}
}

static Screen gCongratsScreen;

Screen* getCongratsScreen() {
	gCongratsScreen = makeScreen(loadTitleScreen, updateTitleScreen);
	return &gCongratsScreen;
};