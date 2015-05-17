#ifndef RECT_H
#define RECT_H

// Basic functionality to test for hits in certain areas of the screen since I'm not using buttons.
// There's probably something like this in the standard library somewhere but it was easier to just
// write it myself than read the docs :)
typedef struct str_rect
{
	int x;
	int y;
	int width; 
	int height;
} rect;

inline bool RectHit(rect rec, double x, double y)
{
	if (
		(x >= rec.x) &&
		(x < (rec.x + rec.width)) &&
		(y >= rec.y) &&
		(y < (rec.y + rec.height))
	)
		return true;

	return false;
}

#endif
