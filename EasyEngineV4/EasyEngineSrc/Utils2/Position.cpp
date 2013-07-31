#include "position.h"

CPosition::CPosition(void)
:
_XPos(0),
_YPos(0)
{
}

CPosition::CPosition(float nPosX, float nPosY)
:
_XPos(nPosX),
_YPos(nPosY)
{
}

CPosition::~CPosition(void)
{
}


float CPosition::GetX() const
{
	return _XPos;
}

float CPosition::GetY() const
{
	return _YPos;
}
	
void CPosition::SetX(float PosX)
{
	_XPos = PosX;
}

void CPosition::SetY(float PosY)
{
	_YPos = PosY;
}

void CPosition::operator+=(CPosition pos)
{
	_XPos += pos._XPos;
	_YPos += pos._YPos;
}


void CPosition::operator*=(float fValue)
{
	_XPos *= fValue;
	_YPos *= fValue;
}



void CPosition::operator/=(float fValue)
{
	_XPos /= fValue;
	_YPos /= fValue;
}


void CPosition::SetPosition(float PosX, float PosY)
{
	_XPos = PosX;
	_YPos = PosY;
}