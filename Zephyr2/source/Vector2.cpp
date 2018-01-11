#include "Vector2.h"

Vector2::Vector2()
{
}

Vector2::Vector2(float xIn, float yIn)
{
	this->x = xIn;
	this->y = yIn;
}

void Vector2::normalize()
{
	float xFinal;
	float yFinal;
	float v;
	v = sqrt((this->x * this->x) + (this->y * this->y));
	xFinal = this->x / v;
	yFinal = this->y / v;
	this->x = xFinal;
	this->y = yFinal;
}

float Vector2::magnitude() {
	return sqrt(pow(this->x, 2) + pow(this->y, 2));
}

Vector2 Vector2::scalarMultiply(float scalar) {
	Vector2 vector;
	vector.x = this->x * scalar;
	vector.y = this->y * scalar;
	return vector;
}

void Vector2::translate(float xOffset, float yOffset)
{
	this->x += xOffset;
	this->y += yOffset;
}

void Vector2::rotate(float angle)
{
	float radians;
	float xTemp, yTemp;
	radians = angle * -1 * (PI / 180);
	xTemp = this->x;
	yTemp = this->y;
	this->x = xTemp * cos(radians) + yTemp * -sin(radians);
	this->y = xTemp * sin(radians) + yTemp * cos(radians);
}

void Vector2::rotateFromOrigin(float originX, float originY, float angle)
{
	float xOffset;
	float yOffset;
	xOffset = originX * -1;
	yOffset = originY * -1;
	this->translate(xOffset, yOffset);
	this->rotate(angle);
	this->translate(originX, originY);
}
