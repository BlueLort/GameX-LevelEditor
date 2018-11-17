
#include <Windows.h>
#include "GameSystem.h"

int main(int argc, char** argv) {
	GameSystem* System=GameSystem::getGameSystemInstance();

	System->run();

	return 0;
}