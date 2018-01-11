#pragma once
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <math.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <cstddef>
#include <mutex>

#define PI 3.14159265358979323846

class Vector2
{
public:
	float x;
	float y;
	Vector2();
	Vector2(float xIn, float yIn);
	void normalize();
	float magnitude();
	Vector2 scalarMultiply(float scalar);
	void translate(float xOffset, float yOffset);
	void rotate(float angle);
	void rotateFromOrigin(float originX, float originY, float angle);
};

