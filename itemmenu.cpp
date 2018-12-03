#include "itemmenu.h"

#include <prism/stlutil.h>

typedef struct {
	int mID;
	int mHasBeenFound;
	int mAnimationID;

} ItemMenuEntry;

static struct {

	vector<vector<ItemMenuEntry> > mEntries;
	
	Vector3DI mSelected;
	int mIsActive;
} gItemMenu;

static void loadItemMenu(void* tData) {
	gItemMenu.mSelected = makeVector3DI(0, 0, 0);
	gItemMenu.mIsActive = 0;

	for (int j = 0; j < 3; j++) {
		vector<ItemMenuEntry> row;
		for (int i = 0; i < 3; i++) {
			ItemMenuEntry e;
			e.mID = j * 3 + i;
			e.mHasBeenFound = 0;
			row.push_back(e);
		}
		gItemMenu.mEntries.push_back(row);
	}
}

static void unloadItemMenu(void* tData) {
	gItemMenu.mEntries.clear();
}

static void updateItemMenu(void* tData) {
	(void)tData;
	if (!gItemMenu.mIsActive) return;
}

ActorBlueprint getItemMenu()
{
	return makeActorBlueprint(loadItemMenu, unloadItemMenu, updateItemMenu);
}
