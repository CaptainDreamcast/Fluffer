#include "stage.h"

#include <prism/blitz.h>

#include "files.h"
#include "player.h"

static struct {

	int mEntityID;
int mEntityID2;
} gData;


static void loadStage(void* tData) {
	(void)tData;

	gData.mEntityID = addBlitzEntity(makePosition(0, 0, 2));
	addBlitzMugenAnimationComponentStatic(gData.mEntityID, getGlobalSprites(), 1, 0);

    gData.mEntityID2 = addBlitzEntity(makePosition(0, 0, 1));
	addBlitzMugenAnimationComponentStatic(gData.mEntityID2, getGlobalSprites(), 1, 1);
    setBlitzEntityScale2D(gData.mEntityID2, 50);
}

static void updateCameraMovement() {
	Position cameraPos = getBlitzCameraHandlerPosition();
	Position playerP = getPlayerPosition();
	Position playerOnScreen = vecSub(playerP, cameraPos);

	Position delta = makePosition(0, 0, 0);
	double left = 150, right = 490;
	if (playerOnScreen.x < left) {
		delta.x = -(left - playerOnScreen.x);
	}
	else if (playerOnScreen.x > right){
		delta.x = playerOnScreen.x - right;
	}

	double up = 400, down = 460;
	if (playerOnScreen.y < up) {
		delta.y = -(up - playerOnScreen.y);
	}
	else if (playerOnScreen.y > down) {
		delta.y = playerOnScreen.y - down;
	}


	Position finalPos = vecAdd(cameraPos, delta);
	finalPos = clampPositionToGeoRectangle(finalPos, makeGeoRectangle(0, 0, 1500 - 640, 1101 - 480));
	setBlitzCameraHandlerPosition(finalPos);
}

static void updateStage(void* tData) {
	updateCameraMovement();

}

ActorBlueprint getStage()
{
	return makeActorBlueprint(loadStage, NULL, updateStage);
}
