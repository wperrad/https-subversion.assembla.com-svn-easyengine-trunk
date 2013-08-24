#ifndef VECTOR_H_
#define VECTOR_H_

class CMatrix;

#include "Utils/IStorage.h"

class  CVector : public IPersistantObject
{

public:		

	float m_x;
	float m_y;
	float m_z;
	float m_w;

	CVector();
	CVector(float x, float y, float z , float w);
	CVector(float x, float y, float z );
	CVector(float TabVal[4]);
	CVector(const CVector& v);
	~CVector();

	CVector operator *(float d) const;
	float operator *(const CVector& v) const;
	CVector operator *(const CMatrix& mat) const;		
	CVector operator+(const CVector& v) const;
	CVector operator-(const CVector& v) const;
	CVector operator/(const float d) const;
	CVector	operator^( const CVector& v );
	float& operator[]( int i );
	bool operator==( const CVector& v ) const;
	bool operator!=( const CVector& v ) const;
	CVector operator-() const;
	CVector& operator +=( const CVector& v );
	CVector& operator -=( const CVector& v );

	float	Norm();
	void	Normalize();
	void	Fill( float x, float y, float z, float w );

	void Store( CBinaryFileStorage& store ) const;
	void Load( CBinaryFileStorage& store );
	void Store( CAsciiFileStorage& store ) const;
	void Load( CAsciiFileStorage& store );
	void Store( CStringStorage& store ) const;
	void Load( CStringStorage& store );


	static void Lerp( const CVector& v1, const CVector& v2, float t, CVector& vOut );
};


#endif //VECTOR_H