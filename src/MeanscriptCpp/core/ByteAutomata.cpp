
#include "MS.h"
namespace meanscriptcore {
using namespace meanscript;

constexpr int32_t MAX_STATES = 32;
constexpr int32_t BUFFER_SIZE = 512;

ByteAutomata::ByteAutomata()
{
	ok = true;
	currentInput = 0;
	currentState = 0;
	stateCounter = 0;
	actionCounter = 0;
	tr.reset( MAX_STATES * 256);
	for (int32_t i=0; i<MAX_STATES * 256; i++) tr[i] = (uint8_t)0xff;

	inputByte = 0;
	index = 0;
	lineNumber = 0;	
	stayNextStep = false;
	running = false;	
	buffer.reset( BUFFER_SIZE);
	tmp.reset( BUFFER_SIZE);
}

ByteAutomata::~ByteAutomata() { }

void ByteAutomata::print ()
{
	for (int32_t i = 0; i <= stateCounter; i++)
	{
		VR("state: ")X(i)XO;

		for (int32_t n = 0; n < 256; n++)
		{
			uint8_t foo = tr[(i * 256) + n];
			if (foo == 0xff) VERBOSEN(".");
			else VERBOSEN(foo);
		}
		VERBOSE("");
	}
}

uint8_t ByteAutomata:: addState (const char * stateName)
{
	stateCounter++;
	stateNames.insert(std::make_pair( (int32_t)stateCounter, stateName));;
	return stateCounter;
}

void ByteAutomata::transition (uint8_t state, const char * input, void (* action)())
{
	uint8_t actionIndex = 0;
	if (action != 0)
	{
		actionIndex = addAction(action);
	}

	uint8_t * bytes = (uint8_t *)input;

	int32_t i = 0;
	while (bytes[i] != 0)
	{
		tr[(state * 256) + bytes[i]] = actionIndex;
		i++;
	}
	//DEBUG(VR("New Transition added: id ")X(actionIndex)XO);
}

void ByteAutomata::fillTransition (uint8_t state, void (* action)())
{
	uint8_t actionIndex = 0;
	if (action != 0) actionIndex = addAction(action);

	for (int32_t i=0; i<256; i++)
	{
		tr[(state * 256) + i] = actionIndex;
	}
	//DEBUG(VR("New Transition filled: id ")X(actionIndex)XO);
}

uint8_t ByteAutomata:: addAction (void (* action)())
{
	actionCounter++;
	actions[actionCounter] = action;
	return actionCounter;
}


void ByteAutomata::next (uint8_t nextState)
{
	currentState = nextState;

	DEBUG(VR("Next state: ")X(stateNames[ (int32_t)currentState])XO);
}


bool ByteAutomata::step (uint8_t input) 
{
	currentInput = input;
	int32_t index = (currentState * 256) + input;
	uint8_t actionIndex = tr[index];

	if (actionIndex == 0) return true; // stay on same state and do nothing else
	if (actionIndex == 0xff||actionIndex < 0)
	{
		CHECK(false, E_UNEXPECTED_CHAR, "char: '" CAT ((char)input) CAT "'");
		ok = false;
		return false; // end
	}

	void (* act)() = actions[actionIndex];

	if (act == 0)
	{
		ASSERT(false, "invalid action index");
	}
	act();
	return true;
}

int32_t ByteAutomata::getIndex ()
{
	return index;
}

uint8_t ByteAutomata::getInputByte ()
{
	return inputByte;
}

void ByteAutomata::stay () 
{
	// same input byte on next step
	ASSERT(!stayNextStep, "'stay' is called twice");
	stayNextStep = true;
}

std::string ByteAutomata::getString (int32_t start, int32_t length) 
{
	CHECK(length < CFG_MAX_NAME_LENGTH, 0, "name is too long");
	
	int32_t i = 0;
	for (; i < length; i++)
	{
		tmp[i] = buffer[start++ % BUFFER_SIZE];
	}

	tmp[i] = '\0';
	return std::string((char*)tmp.get());
}

void ByteAutomata::run (MSInputStream & input) 
{
	inputByte = 0;
	index = 0;
	lineNumber = 1;
	stayNextStep = false;
	running = true;

	while ((!input.end() || stayNextStep) && running && ok)
	{
		if (!stayNextStep)
		{
			index ++;
			inputByte = input.readByte();
			buffer[index % BUFFER_SIZE] = inputByte;
			if (inputByte == '\n') lineNumber++;
		}
		else
		{
			stayNextStep = false;
		}
		VR("[ ")X((char)(inputByte))X(" ]")XO;

		running = step(inputByte);
	}
	
	if (!stayNextStep) index++;
}

void ByteAutomata::printError ()
{
	PR("ERROR: parser state [")X(stateNames[ (int32_t)currentState])X("]")XO;
	PR("Line ")X(lineNumber)X(": \"");
	
	// print nearby code
	int32_t start = index-1;
	while (start > 0 && index - start < BUFFER_SIZE && (char)buffer[start % BUFFER_SIZE] != '\n')
	{
		start --;
	}
	while (++start < index)
	{
		VR((char)(buffer[start % BUFFER_SIZE]));
	}
	PRINT("\"");
}

} // namespace meanscript(core)
// C++ END
