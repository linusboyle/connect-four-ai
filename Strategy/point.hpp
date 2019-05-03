#ifndef POINT_H_
#define POINT_H_

class Point {
public:
	int x;
	int y;

	Point(int x, int y) : x(x), y(y) {}

    bool operator==(const Point& another) {
        return another.x == x && another.y == y;
    }
};

#endif
