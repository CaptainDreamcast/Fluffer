#include "files.h"

static struct {
	MugenSpriteFile mSprites;
	MugenAnimations mAnimations;

} gData;

void loadGlobalFiles()
{
	gData.mSprites = loadMugenSpriteFileWithoutPalette("files/FILES.sff");
	gData.mAnimations = loadMugenAnimationFile("files/GLOBAL.air");
}

MugenSpriteFile * getGlobalSprites()
{
	return &gData.mSprites;
}

MugenAnimations * getGlobalAnimations()
{
	return &gData.mAnimations;
}
