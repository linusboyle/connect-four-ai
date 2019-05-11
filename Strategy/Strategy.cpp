#include "Strategy.h"
#include "game.hpp"
#include <cstring>

#ifdef DEBUG
#include <atlstr.h>
#include <conio.h>
#endif

extern "C" __declspec(dllexport) Point* getPoint(const int M, const int N, const int* top, const int* _board, 
	const int lastX, const int lastY, const int noX, const int noY){

#ifdef DEBUG
	AllocConsole();
#endif

	if (Game::instance().checkRestart(_board, M, N)) {
#ifdef DEBUG
		_cprintf("Game reloaded, size:%d x %d\n", M, N);
#endif
		Game::instance().init(_board, top, M, N, Point{ noX, noY }, { lastX, lastY });
	}

	Point result = Game::instance().getNextStep(Point{ lastX, lastY });
	return new Point(result);
}

extern "C" __declspec(dllexport) void clearPoint(Point* p){
	delete p;
}
