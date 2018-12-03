#include "titlescreen.h"

#include <prism/blitz.h>

#include "adventurescreen.h"

static struct {
	MugenSpriteFile mSprites;

} gData;

static void loadTitleScreen() {
	gData.mSprites = loadMugenSpriteFileWithoutPalette("title/TITLE.sff");
	addMugenAnimation(createOneFrameMugenAnimationForSprite(1, 0), &gData.mSprites, makePosition(0, 0, 1));
	addFadeIn(30, NULL, NULL);
	setWrapperTitleScreen(getTitleScreen());
	playTrack(4);
}


static void gotoAdventureScreen(void* tCaller) {
	(void)tCaller;
	setNewScreen(getAdventureScreen());
}

static void updateTitleScreen() {

	if (hasPressedAFlank() || hasPressedStartFlank()) {
		addFadeOut(30, gotoAdventureScreen, NULL);
	}
}

static Screen gTitleScreen;

Screen* getTitleScreen() {
	gTitleScreen = makeScreen(loadTitleScreen, updateTitleScreen);
	return &gTitleScreen;
};