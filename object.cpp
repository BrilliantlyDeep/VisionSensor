/*
   Yet to be completed!
*/

#include "object.h"

Object::Object(void)
{
}

Object::~Object(void)
{
}

int Object::getXCenter()
{
   return Object::xCenter;
}

int Object::getYCenter()
{
   return Object::yCenter;
}

void Object::setXCenter(int x)
{
	Object::xCenter = x;
}

void Object::setYCenter(int y)
{
	Object::yCenter = y;
}

Scalar Object::getAvgColour()
{
	return Object::AvgColour;
}

void Object::setAvgColour(Scalar min, Scalar max)
{
	// Object::AvgColour = AvgColour( 0.5*abs(max.v0 - min.v0), 0.5*abs(max.v1 - min.v1), 0.5*abs(max.v2 - min.v2) );
	// right now this does not work because the Scalar type is not as easy as it looks
}