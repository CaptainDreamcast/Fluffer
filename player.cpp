#include "player.h"

#include <prism/blitz.h>

#include "collision.h"
#include "files.h"
#include "textboxhandler.h"

static struct {
	int mEntityID;
} gData;


static void loadPlayer(void* tData) {
	(void)tData;

	gData.mEntityID = addBlitzEntity(makePosition(1257, 1098, 10));
	addBlitzPhysicsComponent(gData.mEntityID);
	addBlitzCollisionComponent(gData.mEntityID);
	addBlitzCollisionRect(gData.mEntityID, getPlayerCollisionList(), makeCollisionRect(makePosition(-20, -20, 0), makePosition(20, 20, 0)));
	addBlitzMugenAnimationComponent(gData.mEntityID, getGlobalSprites(), getGlobalAnimations(), 100);

	setBlitzCameraHandlerPosition(makePosition(1000, 1000, 0));
}

static void updatePlayerMovement() {
	if (isTextDisplayActive()) return;

	Vector3DI tTarget = makeVector3DI(0, 0, 0);
	int speed = 4;

	if (hasPressedB()) {
		speed = 8;
	}

	if (hasPressedLeft()) {
		tTarget.x = -speed;
	}
	else if (hasPressedRight()) {
		tTarget.x = speed;
	}

	if (hasPressedUp()) {
		tTarget.y = -speed;
	}
	else if (hasPressedDown()) {
		tTarget.y = speed;
	}

	Position* p = getBlitzEntityPositionReference(gData.mEntityID);
	p->x += tTarget.x;
	p->y += tTarget.y;

	*p = clampPositionToGeoRectangle(*p, makeGeoRectangle(0, 200, 1500, 1101));
	p->z = 10 + p->y / 1101;

	if (tTarget.x < 0) setBlitzMugenAnimationFaceDirection(gData.mEntityID, 0);
	if (tTarget.x > 0) setBlitzMugenAnimationFaceDirection(gData.mEntityID, 1);

	int isMoving = tTarget.x || tTarget.y;
	if (isMoving && getBlitzMugenAnimationAnimationNumber(gData.mEntityID) == 100) changeBlitzMugenAnimation(gData.mEntityID, 120);
	else if(!isMoving && getBlitzMugenAnimationAnimationNumber(gData.mEntityID) == 120) changeBlitzMugenAnimation(gData.mEntityID, 100);
}

static void updatePlayer(void* tData) {
	updatePlayerMovement();
}

ActorBlueprint getPlayer()
{
	return makeActorBlueprint(loadPlayer, NULL, updatePlayer);
}

Position getPlayerPosition()
{
	return getBlitzEntityPosition(gData.mEntityID);
}

void setPlayerQuiet()
{
	changeBlitzMugenAnimation(gData.mEntityID, 100);
}
