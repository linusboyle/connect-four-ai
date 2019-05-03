#include "Strategy.h"
#include "UCT.hpp"
#include <cstring>

#ifndef CUSTOM_MAIN
#include <conio.h>
#include <atlstr.h>
#endif

using namespace std;

/*
	策略函数接口,该函数被对抗平台调用,每次传入当前状态,要求输出你的落子点,该落子点必须是一个符合游戏规则的落子点,不然对抗平台会直接认为你的程序有误
	
	input:
		为了防止对对抗平台维护的数据造成更改，所有传入的参数均为const属性
		M, N : 棋盘大小 M - 行数 N - 列数 均从0开始计， 左上角为坐标原点，行用x标记，列用y标记
		top : 当前棋盘每一列列顶的实际位置. e.g. 第i列为空,则_top[i] == M, 第i列已满,则_top[i] == 0
		_board : 棋盘的一维数组表示, 为了方便使用，在该函数刚开始处，我们已经将其转化为了二维数组board
				你只需直接使用board即可，左上角为坐标原点，数组从[0][0]开始计(不是[1][1])
				board[x][y]表示第x行、第y列的点(从0开始计)
				board[x][y] == 0/1/2 分别对应(x,y)处 无落子/有用户的子/有程序的子,不可落子点处的值也为0
		lastX, lastY : 对方上一次落子的位置, 你可能不需要该参数，也可能需要的不仅仅是对方一步的
				落子位置，这时你可以在自己的程序中记录对方连续多步的落子位置，这完全取决于你自己的策略
		noX, noY : 棋盘上的不可落子点(注:其实这里给出的top已经替你处理了不可落子点，也就是说如果某一步
				所落的子的上面恰是不可落子点，那么UI工程中的代码就已经将该列的top值又进行了一次减一操作，
				所以在你的代码中也可以根本不使用noX和noY这两个参数，完全认为top数组就是当前每列的顶部即可,
				当然如果你想使用lastX,lastY参数，有可能就要同时考虑noX和noY了)
		以上参数实际上包含了当前状态(M N _top _board)以及历史信息(lastX lastY),你要做的就是在这些信息下给出尽可能明智的落子点
	output:
		你的落子点Point
*/

//static void printBoard(int* board, int row, int column) {
//	_cprintf("received board:\n");
//	for (int i = 0; i < row; ++i) {
//		for (int j = 0; j < column; ++j) {
//			_cprintf("%d ", board[i * column + j]);
//		}
//		_cprintf("\n");
//	}
//}

//static void printTops(int* tops, int column) {
//	_cprintf("received tops:\n");
//	for (int i = 0; i < column; ++i) {
//		_cprintf("%d ", tops[i]);
//	}
//	_cprintf("\n");
//}

extern "C" __declspec(dllexport) Point* getPoint(const int M, const int N, const int* top, const int* _board, 
	const int lastX, const int lastY, const int noX, const int noY){
	UCTTree t(M, N, { noX, noY });
#ifndef CUSTOM_MAIN
	AllocConsole();
#endif
	// _cprintf("%d %d\n", M, N);

	int* board = new int[sizeof(int) * M * N];
	int* tops = new int[sizeof(int) * N];
	std::memcpy(board, _board, sizeof(int) * M * N);
	std::memcpy(tops, top, sizeof(int) * N);

	// printBoard(board, M, N);
	// printTops(tops, N);

	Point pos = t.search(board, tops, { lastX, lastY });
	return new Point(pos.x, pos.y);
}


extern "C" __declspec(dllexport) void clearPoint(Point* p){
	delete p;
	return;
}
