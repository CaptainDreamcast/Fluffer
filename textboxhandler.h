#pragma once

#include <prism/blitz.h>
#include <prism/actorhandler.h>

ActorBlueprint getTextboxHandler();

int isTextDisplayActive();
void setTextBoxDisplayActive(int tID, Position tPosition, int tOwnerEnemyID);
int registerTextHandling(MugenDefScript* tScript);