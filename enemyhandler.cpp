#include "enemyhandler.h"

#include <prism/blitz.h>

#include "collision.h"
#include "player.h"
#include "textboxhandler.h"
#include "files.h"

typedef struct {
	int mEntityID;

	int mIsShowingPressA;
	int mPressAEntity;

	int mTextHandlerID;
	int mID;

	string mName;
	string mDisplayName;
} Enemy;

static struct {
	int gCounter;
	int mHasForcedConversation;

	map<int, Enemy> mEnemies;
} gEnemyHandler;

static void addEnemy(string tName, Position tPos, int tStartAnimation, string tDisplayName) {
	Enemy e;

	string path = "enemies/" + tName + ".def";
	MugenDefScript script = loadMugenDefScript(path);
	e.mTextHandlerID = registerTextHandling(&script);
	unloadMugenDefScript(script);

	tPos.z = 10 + tPos.y / 1101;

	e.mEntityID = addBlitzEntity(tPos);
	addBlitzCollisionComponent(e.mEntityID);
	int collisionID = addBlitzCollisionRect(e.mEntityID, getNonPlayerCollisionList(), makeCollisionRect(makePosition(-20, -20, 0), makePosition(20, 20, 1)));
	setBlitzCollisionSolid(e.mEntityID, collisionID, 0);
	addBlitzMugenAnimationComponent(e.mEntityID, getGlobalSprites(), getGlobalAnimations(), tStartAnimation);
	e.mIsShowingPressA = 0;
	e.mID = gEnemyHandler.gCounter++;
	
	char bug[1024];
	strcpy(bug, tName.data());
	turnStringLowercase(bug);
	e.mName = string(bug);
	e.mDisplayName = tDisplayName;

	gEnemyHandler.mEnemies[e.mID] = e;
}

static void loadEnemyHandler(void* tData) {
	gEnemyHandler.mEnemies.clear();
	gEnemyHandler.mHasForcedConversation = 0;

	addEnemy("FLUFFER", makePosition(1180, 994, 10), 300, "Mr. Fluffers");
	addEnemy("TIMMY", makePosition(1358, 987, 10), 400, "Junior");
	addEnemy("CAULDRON", makePosition(1288, 926, 10), 500, "");

	addEnemy("NOTTA", makePosition(779, 961, 10), 590, "Notta Vitch");
	addEnemy("CAT", makePosition(646, 968, 10), 560, "Lasagna");


	addEnemy("PEINT", makePosition(324, 961, 10), 540, "Finga Peint");
	addEnemy("PEINTDOG", makePosition(132, 955, 10), 550, "Kola");

	addEnemy("NARUTO", makePosition(1248, 424, 10), 530, "???");
	addEnemy("NARUTODOG", makePosition(1390, 420, 10), 600, "???");

	addEnemy("HIT", makePosition(1008, 418, 10), 580, "Hit Zebelle");
	addEnemy("HITDOG", makePosition(869, 416, 10), 520, "DoPro");

	addEnemy("HAUTAIRE", makePosition(635, 425, 10), 510, "Madame Hautaire");
	addEnemy("HAUTAIREDOG", makePosition(481, 426, 10), 570, "Pudding the Poudle");

	addEnemy("BLACKENWHITE", makePosition(194, 421, 10), 200, "Judge Blackenwhite");
}

static void unloadEnemyHandler(void* tData) {
	gEnemyHandler.mEnemies.clear();
}



static void updateSingleEnemySetPressAActive(Enemy& tEnemy) {
	if (tEnemy.mIsShowingPressA) return;
	if (isTextDisplayActive()) return;
	Position playerPos = getPlayerPosition();
	Position enemyPos = getBlitzEntityPosition(tEnemy.mEntityID);
	if (getDistance2D(playerPos, enemyPos) > 70) return;

	tEnemy.mPressAEntity = addBlitzEntity(vecAdd(enemyPos, makePosition(40, -100, 11)));
	addBlitzMugenAnimationComponent(tEnemy.mPressAEntity, getGlobalSprites(), getGlobalAnimations(), 10000);
	tEnemy.mIsShowingPressA = 1;

}

static void setSingleEnemySetPressAInactive(Enemy& tEnemy) {
	removeBlitzEntity(tEnemy.mPressAEntity);
	tEnemy.mIsShowingPressA = 0;
}

static void updateSingleEnemySetPressAInactive(Enemy& tEnemy) {
	if (!tEnemy.mIsShowingPressA) return;
	Position playerPos = getPlayerPosition();
	Position enemyPos = getBlitzEntityPosition(tEnemy.mEntityID);
	if (getDistance2D(playerPos, enemyPos) <= 70) return;

	setSingleEnemySetPressAInactive(tEnemy);
}

static void updateEnemyTalkTo(Enemy& tEnemy) {
	if (!tEnemy.mIsShowingPressA) return;
	if (isTextDisplayActive()) return;

	if (hasPressedAFlank()) {
		setPlayerQuiet();
		setTextBoxDisplayActive(tEnemy.mTextHandlerID, makePosition(0, 0, 0), tEnemy.mID);
		setSingleEnemySetPressAInactive(tEnemy);
	}
}

static void updateEnemyForce(Enemy& tEnemy) {
	if (!gEnemyHandler.mHasForcedConversation&& tEnemy.mName == "timmy") {
		setPlayerQuiet();
		setTextBoxDisplayActive(tEnemy.mTextHandlerID, makePosition(0, 0, 0), tEnemy.mID);
		gEnemyHandler.mHasForcedConversation = 1;
	}

}

static void updateSingleEnemy(void* tCaller, Enemy& tEnemy) {
	(void*)tCaller;
	updateSingleEnemySetPressAActive(tEnemy);
	updateSingleEnemySetPressAInactive(tEnemy);
	updateEnemyTalkTo(tEnemy);
	updateEnemyForce(tEnemy);
}

static void updateEnemyHandler(void* tData) {
	stl_int_map_map(gEnemyHandler.mEnemies, updateSingleEnemy);
}

ActorBlueprint getEnemyHandler()
{
	return makeActorBlueprint(loadEnemyHandler, unloadEnemyHandler, updateEnemyHandler);
}

Position getEnemyPosition(int tID)
{
	Enemy& e = gEnemyHandler.mEnemies[tID];
	return getBlitzEntityPosition(e.mEntityID);
}

typedef struct {
	string mTest;
	Position mFound;
	string mDisplayName;

	int mIsChangingAnimation;
	int mAnimationID;
	string mNewName;

	int mIsSettingPosition;
	Position tPosition;
} GetEnemyPositionCaller;

static void checkSingleEnemy(GetEnemyPositionCaller* tCaller, Enemy& tEnemy) {
	if (tEnemy.mName == tCaller->mTest) {
		tCaller->mFound = getBlitzEntityPosition(tEnemy.mEntityID);
		tCaller->mDisplayName = tEnemy.mDisplayName;
		if (tCaller->mIsChangingAnimation) {
			changeBlitzMugenAnimation(tEnemy.mEntityID, tCaller->mAnimationID);
			tEnemy.mDisplayName = tCaller->mNewName;
			Position* p = getBlitzEntityPositionReference(tEnemy.mEntityID);
			p->z += 10;
		}
		if (tCaller->mIsSettingPosition) {
			
			Position* p = getBlitzEntityPositionReference(tEnemy.mEntityID);
			p->x = tCaller->tPosition.x;
			p->y = tCaller->tPosition.y;
		}
	}
}

Position getEnemyPosition(std::string tName)
{
	GetEnemyPositionCaller caller;
	caller.mTest = tName;
	caller.mFound = makePosition(0, 0, 0);
	caller.mIsChangingAnimation = 0;
	caller.mIsSettingPosition = 0;
	stl_int_map_map(gEnemyHandler.mEnemies, checkSingleEnemy, &caller);
	return caller.mFound;
}

std::string getEnemyName(int tID)
{
	Enemy& e = gEnemyHandler.mEnemies[tID];
	return e.mDisplayName;
}

std::string getEnemyName(std::string tName)
{
	GetEnemyPositionCaller caller;
	caller.mTest = tName;
	caller.mFound = makePosition(0, 0, 0);
	caller.mIsChangingAnimation = 0;
	caller.mIsSettingPosition = 0;
	stl_int_map_map(gEnemyHandler.mEnemies, checkSingleEnemy, &caller);
	return caller.mDisplayName;
}

void setEnemyAnimation(std::string tName, int tAnimationID, std::string tNewName)
{
	GetEnemyPositionCaller caller;
	caller.mTest = tName;
	caller.mFound = makePosition(0, 0, 0);
	caller.mIsChangingAnimation = 1;
	caller.mIsSettingPosition = 0;
	caller.mAnimationID = tAnimationID;
	caller.mNewName = tNewName;
	stl_int_map_map(gEnemyHandler.mEnemies, checkSingleEnemy, &caller);
}

void setEnemyPosition(std::string tName, Position tPosition)
{
	GetEnemyPositionCaller caller;
	caller.mTest = tName;
	caller.mFound = makePosition(0, 0, 0);
	caller.mIsChangingAnimation = 0;
	caller.mIsSettingPosition = 1;
	caller.tPosition = tPosition;
	stl_int_map_map(gEnemyHandler.mEnemies, checkSingleEnemy, &caller);

}
