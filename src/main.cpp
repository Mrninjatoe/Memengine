#include "engine.hpp"
#include <time.h>

int main(int argcc, char* argv[]) {
	srand(1);
	printf("#####Memengine Has Been Started!#####\n");
	Engine::getInstance()->run();
	printf("#####Memengine Has Been Terminated!####\n");
	return 0;
}