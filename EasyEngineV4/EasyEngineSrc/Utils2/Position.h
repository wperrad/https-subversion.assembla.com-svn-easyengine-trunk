#ifndef POSITION_H
#define POSITION_H


class  CPosition
{

	float 					_XPos;
	float 					_YPos;

public:
							CPosition(void);
							CPosition(float nPosX, float nPosY);
	virtual					~CPosition(void);
	float 					GetX() const;
	float 					GetY() const;
	void					SetX(float nx);
	void					SetY(float ny);
	void					SetPosition(float PosX, float PosY);
	void					operator+=(CPosition pos);
	void					operator*=(float fValue);
	void					operator/=(float fValue);
};


#endif //POSITION_H