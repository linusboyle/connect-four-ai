#include "Strategy.h"
#include "game.hpp"
#include <cstring>

#ifndef CUSTOM_MAIN
#include <atlstr.h>
#endif

extern "C" __declspec(dllexport) Point* getPoint(const int M, const int N, const int* top, const int* _board, 
	const int lastX, const int lastY, const int noX, const int noY){

	// if (Game::checkRestart(_board, M, N)) {
		Game::instance().init(_board, top, M, N, Point{ noX, noY });
	// }

#ifndef CUSTOM_MAIN
	AllocConsole();
#endif

	Point result = Game::instance().getNextStep(Point{ lastX, lastY });
	return new Point(result);
}

extern "C" __declspec(dllexport) void clearPoint(Point* p){
	delete p;
}
