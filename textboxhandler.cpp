#include "textboxhandler.h"

#include <algorithm>

#include <prism/blitz.h>
#include <prism/mugenassignment.h>
#include <prism/mugenassignmentevaluator.h>
#include <prism/memoryhandler.h>

#include "globalvar.h"
#include "files.h"
#include "player.h"
#include "enemyhandler.h"
#include "congratsscreen.h"

class TextStep {

public:
	static void loadStepAndReturnID(MugenDefScriptGroup* tGroup, map<int, TextStep*>& tSteps);

	virtual int handleAndReturnIfOver(int& tStep, Position tPosition) = 0;

};

typedef struct {
	map<int, TextStep*> mSteps;
} RegisteredText;

typedef enum {
	SPEECH_BUBBLE_THINKING,
	SPEECH_BUBBLE_SPEAKING,
	SPEECH_BUBBLE_NARRATION,

} SpeechBubbleType;



class OptionsStep : public TextStep {
private:
	class Option {
	public:
		MugenAssignment* mAssignment;
		string mText;
		int mNext;
	public:

		Option(string tText, MugenAssignment* tDependency, int tNext) {
			this->mText = tText;
			this->mAssignment = tDependency;
			this->mNext = tNext;
		}

	};
	vector<Option> mOptions;
	int mIsActive;

	int mSelectedOption;

public:
	OptionsStep(MugenDefScriptGroup* tGroup) {
		for (int i = 0; i < 10; i++) {
			char name[100];

			string var = "option" + i;
			var += ".assignment";
			strcpy(name, var.data());

			char* allocatedText = getAllocatedMugenDefStringOrDefaultAsGroup(tGroup, name, "1");
			MugenAssignment* assignment = parseMugenAssignmentFromString(allocatedText);
			freeMemory(allocatedText);

			var = "option" + i;
			var += ".text";
			strcpy(name, var.data());
			allocatedText = getAllocatedMugenDefStringOrDefaultAsGroup(tGroup, name, "1");
			string optionText(allocatedText);
			freeMemory(allocatedText);

			var = "option" + i;
			var += ".next";
			strcpy(name, var.data());
			int next = getMugenDefIntegerOrDefaultAsGroup(tGroup, name, 0);

			this->mOptions.push_back(Option(optionText, assignment, next));
		}

	}

	virtual int handleAndReturnIfOver(int& tStep, Position tPosition) {
		if (!this->mIsActive) {
			setOptionActive(tPosition);
		}

		if (hasPressedAFlank()) {
			setOptionInactive();
			return 0;
		}

		return 1;
	}

private:
	void setOptionActive(Position tPosition) {
		this->mIsActive = 1;
	}

	void setOptionInactive() {
		this->mIsActive = 0;
	}

};

class BranchStep : public TextStep {
	MugenAssignment* mAssignment;
	MugenAssignment* mTrueStep;
	MugenAssignment* mFalseStep;

public:
	BranchStep(MugenDefScriptGroup* tGroup) {

		char* assignment = getAllocatedMugenDefStringOrDefaultAsGroup(tGroup, "dependency", "1");
		this->mAssignment = parseMugenAssignmentFromString(assignment);
		freeMemory(assignment);

		assignment = getAllocatedMugenDefStringOrDefaultAsGroup(tGroup, "false", "0");
		this->mFalseStep = parseMugenAssignmentFromString(assignment);
		freeMemory(assignment);

		assignment = getAllocatedMugenDefStringOrDefaultAsGroup(tGroup, "true", "0");
		this->mTrueStep = parseMugenAssignmentFromString(assignment);
		freeMemory(assignment);
	}

	virtual int handleAndReturnIfOver(int& tStep, Position tPosition) {
		int eval = evaluateMugenAssignment(this->mAssignment, NULL);
		if (eval) tStep = evaluateMugenAssignmentAndReturnAsInteger(this->mTrueStep, NULL);
		else tStep = evaluateMugenAssignmentAndReturnAsInteger(this->mFalseStep, NULL);

		return 0;
	}

};

class VariableSetStep : public TextStep {
	
	int mID;
	MugenAssignment* mValue;

public:
	VariableSetStep(MugenDefScriptGroup* tGroup) {
		this->mID = getMugenDefIntegerOrDefaultAsGroup(tGroup, "id", 0);

		char* assignment = getAllocatedMugenDefStringOrDefaultAsGroup(tGroup, "value", "0");
		this->mValue = parseMugenAssignmentFromString(assignment);
		freeMemory(assignment);
	}

	virtual int handleAndReturnIfOver(int& tStep, Position tPosition) {
		int value = evaluateMugenAssignmentAndReturnAsInteger(this->mValue, NULL);
		setGlobalVar(this->mID, value);
		tStep++;
		return 0;
	}
};

static void setTextBoxDisplayInactive();

static void gotoCongrats(void* tData) {
	(void)tData;
	setNewScreen(getCongratsScreen());
}

class OverStep : public TextStep {
public:
	OverStep(MugenDefScriptGroup* tGroup) {
	}

	virtual int handleAndReturnIfOver(int& tStep, Position tPosition) {
		setTextBoxDisplayInactive();
		return 1;
	}
};


class GameOverStep : public TextStep {
public:
	GameOverStep(MugenDefScriptGroup* tGroup) {
	}

	virtual int handleAndReturnIfOver(int& tStep, Position tPosition) {
		addFadeOut(30, gotoCongrats, NULL);
		setTextBoxDisplayInactive();
		tStep = 9999;
		return 1;
	}
};

#include <prism/screeneffect.h>

class BlackOutStep : public TextStep {
public:
	BlackOutStep(MugenDefScriptGroup* tGroup) {
	}

	virtual int handleAndReturnIfOver(int& tStep, Position tPosition) {
		addMugenAnimation(getMugenAnimation(getGlobalAnimations(), 30000), getGlobalSprites(), makePosition(0, 0, 35));
		tStep += 5;
		return 0;
	}
};

class ChangeAnimationStep : public TextStep {

private:
	string mName;
	string mNewName;
	int mAnimationNumber;
	int mNext;
public:
	ChangeAnimationStep(MugenDefScriptGroup* tGroup) {
		char* assignment = getAllocatedMugenDefStringOrDefaultAsGroup(tGroup, "enemy", " ");
		turnStringLowercase(assignment);
		this->mName = string(assignment);
		freeMemory(assignment);

		assignment = getAllocatedMugenDefStringOrDefaultAsGroup(tGroup, "newname", " ");
		this->mNewName = string(assignment);
		freeMemory(assignment);

		this->mAnimationNumber = getMugenDefIntegerOrDefaultAsGroup(tGroup, "anim", 0);
		this->mNext = getMugenDefIntegerOrDefaultAsGroup(tGroup, "next", 0);

	}

	virtual int handleAndReturnIfOver(int& tStep, Position tPosition) {
		setEnemyAnimation(this->mName, this->mAnimationNumber, this->mNewName);
		tStep = this->mNext;
		return 0;
	}
};

class ChangePosStep : public TextStep {

private:
	string mName;
	Position mPosition;
	int mNext;
public:
	ChangePosStep(MugenDefScriptGroup* tGroup) {
		char* assignment = getAllocatedMugenDefStringOrDefaultAsGroup(tGroup, "enemy", " ");
		turnStringLowercase(assignment);
		this->mName = string(assignment);
		freeMemory(assignment);



		this->mPosition = getMugenDefVectorOrDefaultAsGroup(tGroup, "pos", makePosition(0,0,0));
		this->mNext = getMugenDefIntegerOrDefaultAsGroup(tGroup, "next", 0);

	}

	virtual int handleAndReturnIfOver(int& tStep, Position tPosition) {
		setEnemyPosition(this->mName, this->mPosition);
		tStep = this->mNext;
		return 0;
	}
};

class DummyStep : public TextStep {
public:
	DummyStep(MugenDefScriptGroup* tGroup) {
	}

	virtual int handleAndReturnIfOver(int& tStep, Position tPosition) {
		return 1;
	}
};

static struct {

	int mCurrentStep;

	Position mActiveTextPosition;
	int mActiveText;
	int mOwnerEnemyID;
	int mIsActive;

	int mPreviousLoadID;

	vector<RegisteredText> mTexts;
} gTextBoxHandler;


class SingleTextStep : public TextStep {
private:
	string mText;
	SpeechBubbleType mType;

	int mBackgroundAnimationID;
	int mSelectorID;
	int mNameTextID;
	int mIsActive;
	int mNext;

	string mSpeaker;

	int mTextID;
public:
	SingleTextStep(MugenDefScriptGroup* tGroup) {
		char* allocatedText = getAllocatedMugenDefStringOrDefaultAsGroup(tGroup, "text", "");
		this->mText = string(allocatedText);
		freeMemory(allocatedText);

		allocatedText = getAllocatedMugenDefStringOrDefaultAsGroup(tGroup, "type", "");
		this->mType = getTypeFromString(allocatedText);
		freeMemory(allocatedText);

		allocatedText = getAllocatedMugenDefStringOrDefaultAsGroup(tGroup, "speaker", "");
		turnStringLowercase(allocatedText);
		this->mSpeaker = string(allocatedText);
		freeMemory(allocatedText);

		this->mNext = getMugenDefIntegerOrDefaultAsGroup(tGroup, "next", -1);

		this->mIsActive = 0;
	}

	virtual int handleAndReturnIfOver(int& tStep, Position tPosition) {
		if (!this->mIsActive) {
			setTextActive(tPosition);
		}

		if (hasPressedAFlank()) {
			if (!isMugenTextBuiltUp(this->mTextID)) {
				setMugenTextBuiltUp(this->mTextID);
			}
			else {
				setTextInactive();
				if (this->mNext == -1) tStep = tStep + 10;
				else tStep = this->mNext;
				return 0;
			}
		}

		return 1;
	}

private:
	void setTextActive(Position tPos) {
		char text[1024];
		strcpy(text, this->mText.data());
		this->mTextID = addMugenTextMugenStyle(text, makePosition(80, 80, 40), makeVector3DI(1, 7, 1));
		setMugenTextBuildup(this->mTextID, 1);
		setMugenTextTextBoxWidth(this->mTextID, 520);

		Position p;
		int animationID;
		int selector = -1;
		if (this->mType == SPEECH_BUBBLE_SPEAKING) {
			animationID = 20000;
			p = makePosition(320, 20, 39);
			selector = 20001;
		}
		else if (this->mType == SPEECH_BUBBLE_THINKING) {
			animationID = 20010;
			p = makePosition(320, 20, 39);
			selector = 20011;
		}
		else {
			animationID = 20020;
			p = makePosition(320, 20, 39);
		}
		this->mBackgroundAnimationID = addMugenAnimation(getMugenAnimation(getGlobalAnimations(), animationID), getGlobalSprites(), p);

		if (selector != -1) {
			double x;
			string name;

			if (this->mSpeaker == "player") {
				x = getPlayerPosition().x;
				name = "Douglas Dogsworth";
			}
			else if (this->mSpeaker == "enemy") {
				Position p = getEnemyPosition(gTextBoxHandler.mOwnerEnemyID);
				x = p.x;
				name = getEnemyName(gTextBoxHandler.mOwnerEnemyID);
			}
			else {
				Position p = getEnemyPosition(this->mSpeaker);
				x = p.x;
				name = getEnemyName(this->mSpeaker);
			}

			Position offset = getBlitzCameraHandlerPosition();
			x -= offset.x;
			Position pos = makePosition(x, p.y + 165, p.z - 1);
			this->mSelectorID = addMugenAnimation(getMugenAnimation(getGlobalAnimations(), selector), getGlobalSprites(), pos);

			char text[1024];
			strcpy(text, name.data());
			this->mNameTextID = addMugenTextMugenStyle(text, makePosition(80, 50, 40), makeVector3DI(1, 7, 1));
		}
		else {
			this->mSelectorID = -1;
		}

		this->mIsActive = 1;

	}

	void setTextInactive() {
		removeMugenText(this->mTextID);
		removeMugenAnimation(this->mBackgroundAnimationID);
		if (this->mSelectorID != -1) {
			removeMugenAnimation(this->mSelectorID);
			removeMugenText(this->mNameTextID);
		}
		this->mIsActive = 0;
	}

	SpeechBubbleType getTypeFromString(char* tType) {
		turnStringLowercase(tType);
		string s(tType);
		if (s == "thinking") return SPEECH_BUBBLE_THINKING;
		else if (s == "speaking") return SPEECH_BUBBLE_SPEAKING;
		else return SPEECH_BUBBLE_NARRATION;
	}
};



void TextStep::loadStepAndReturnID(MugenDefScriptGroup* tGroup, map<int, TextStep*>& tSteps) {
	string name(tGroup->mName);
	string firstWord = name.substr(0, name.find(' '));
	std::transform(firstWord.begin(), firstWord.end(), firstWord.begin(), ::tolower);
	int id;
	
	string numberWord = name.substr(name.find(' ') + 1);
	id = atoi(numberWord.data());

	gTextBoxHandler.mPreviousLoadID = id;

	if (firstWord == "text") {
		tSteps[id] = new SingleTextStep(tGroup);
	}
	else if(firstWord == "option") {
		tSteps[id] = new OptionsStep(tGroup);
	}
	else if (firstWord == "branch") {
		tSteps[id] = new BranchStep(tGroup);
	}
	else if (firstWord == "varset" || firstWord == "setvar") {
		tSteps[id] = new VariableSetStep(tGroup);
	}
	else if (firstWord == "over") {
		tSteps[id] = new OverStep(tGroup);
	}
	else if (firstWord == "gameover") {
		tSteps[id] = new GameOverStep(tGroup);
	}
	else if (firstWord == "blackout") {
		tSteps[id] = new BlackOutStep(tGroup);
	}
	else if (firstWord == "changeanim") {
		tSteps[id] = new ChangeAnimationStep(tGroup);
	}
	else if (firstWord == "setenemypos") {
		tSteps[id] = new ChangePosStep(tGroup);
	}
	else {
		logErrorFormat("Unable to parse group %s %s %d", name.data(), firstWord.data(), id);
	}
}

static void evalPlayerFunction(char* tOutput, void* tCaller) {
	Position p = getPlayerPosition();
	double x = p.x;

	sprintf(tOutput, "%f", x);
}

static void evalEnemyFunction(char* tOutput, void* tCaller) {
	Position p = getEnemyPosition(gTextBoxHandler.mOwnerEnemyID);
	double x = p.x;

	sprintf(tOutput, "%f", x);
}

static void evalVarFunction(char* tOutput, void* tCaller, char* tIndex) {
	int index = atoi(tIndex);
	int val = getGlobalVar(index);

	sprintf(tOutput, "%d", val);
}

static void loadAssignments() {
	resetMugenAssignmentContext();
	addMugenAssignmentVariable("player", evalPlayerFunction);
	addMugenAssignmentVariable("enemy", evalEnemyFunction);
	addMugenAssignmentArray("var", evalVarFunction);
}

static void loadTextBoxHandler(void* tData) {
	(void)tData;
	gTextBoxHandler.mIsActive = 0;
	gTextBoxHandler.mTexts.clear();
	loadAssignments();
}

static void unloadTextBoxHandler(void* tData) {
	gTextBoxHandler.mTexts.clear();
}

static void updateTextBoxHandler(void* tData) {
	(void)tData;
	if (!gTextBoxHandler.mIsActive) return;

	RegisteredText& text = gTextBoxHandler.mTexts[gTextBoxHandler.mActiveText];

	int isOver = 0;
	while (!isOver) {
		TextStep* step = text.mSteps[gTextBoxHandler.mCurrentStep];
		isOver = step->handleAndReturnIfOver(gTextBoxHandler.mCurrentStep, gTextBoxHandler.mActiveTextPosition);
	}
}

ActorBlueprint getTextboxHandler()
{
	return makeActorBlueprint(loadTextBoxHandler, unloadTextBoxHandler, updateTextBoxHandler);
}

int isTextDisplayActive()
{
	return gTextBoxHandler.mIsActive;
}

void setTextBoxDisplayActive(int tID, Position tPosition, int tOwnerEnemyID)
{
	gTextBoxHandler.mActiveTextPosition = tPosition;
	gTextBoxHandler.mCurrentStep = 0;
	gTextBoxHandler.mActiveText = tID;
	gTextBoxHandler.mOwnerEnemyID = tOwnerEnemyID;
	gTextBoxHandler.mIsActive = 1;
}

static void setTextBoxDisplayInactive() {
	gTextBoxHandler.mIsActive = 0;
}

int registerTextHandling(MugenDefScript * tScript)
{
	RegisteredText e;
	e.mSteps = map<int, TextStep*>();

	gTextBoxHandler.mPreviousLoadID = -1;

	MugenDefScriptGroup* tGroup = tScript->mFirstGroup;
	while (tGroup) {
		TextStep::loadStepAndReturnID(tGroup, e.mSteps);
		tGroup = tGroup->mNext;
	}

	gTextBoxHandler.mTexts.push_back(e);
	return gTextBoxHandler.mTexts.size() - 1;
}
