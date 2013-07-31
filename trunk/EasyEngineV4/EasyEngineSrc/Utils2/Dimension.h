#ifndef DIMENSION_H
#define DIMENSION_H


class  CDimension
{
	float				_fWidth;
	float				_fHeight;

public:
						CDimension(void);
						CDimension(float nWidth, float nHeight);
	virtual 			~CDimension(void);
	void				SetWidth(float nWidth);
	void				SetHeight(float nHeight);
	void				SetDimension(float nWidth, float nHeight);
	void				SetDimension(CDimension d);
	float				GetWidth()const;
	float				GetHeight()const;

	

};



#endif //DIMENSION_H