#pragma once

#include <prism/actorhandler.h>

ActorBlueprint getGlobalVarHandler();

void setGlobalVar(int tIndex, int tValue);

int getGlobalVar(int tIndex);