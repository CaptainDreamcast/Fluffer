#pragma once

#include <string>

#include <prism/actorhandler.h>
#include <prism/geometry.h>

ActorBlueprint getEnemyHandler();

Position getEnemyPosition(int tID);
Position getEnemyPosition(std::string tName);

std::string getEnemyName(int tID);
std::string getEnemyName(std::string tName);
void setEnemyAnimation(std::string tName, int tAnimationID, std::string tNewName);
void setEnemyPosition(std::string tName, Position tPosition);