#include "box.h"
#include "IRenderer.h"
#include "Cylinder.h"

CBox::CBox():
m_bInitialized( false )
{
}

CBox::CBox( CVector& oMinPoint, CVector& oDimension ):
m_oMinPoint( oMinPoint ),
m_oDimension( oDimension ),
m_bInitialized( true )
{
	m_fBoundingSphereRadius = ComputeBoundingSphereRadius();
}

CBox::CBox( const CBox& oBox )
{
	m_oMinPoint = oBox.m_oMinPoint;
	m_oDimension = oBox.m_oDimension;
	m_oTM = oBox.m_oTM;
}

void CBox::Set( const CVector& oMinPoint, const CVector& oDimension )
{
	m_oMinPoint = oMinPoint;
	m_oDimension = oDimension;
	m_fBoundingSphereRadius = ComputeBoundingSphereRadius();
	m_bInitialized = true;
}

void CBox::GetCenter( CVector& oCenter ) const
{
	oCenter = m_oMinPoint +  m_oDimension / 2.f;
}

void CBox::AddPoint( const CVector& p )
{
	if( !m_bInitialized )
	{
		m_oMinPoint = p;
		m_bInitialized = true;
	}

	CVector vMax = m_oMinPoint + m_oDimension;
	if( vMax.m_x < p.m_x )
		vMax.m_x = p.m_x;
	if( vMax.m_y < p.m_y )
		vMax.m_y = p.m_y;
	if( vMax.m_z < p.m_z )
		vMax.m_z = p.m_z;

	if ( p.m_x < m_oMinPoint.m_x )
		m_oMinPoint.m_x = p.m_x;
	if ( p.m_y < m_oMinPoint.m_y )
		m_oMinPoint.m_y = p.m_y;
	if ( p.m_z < m_oMinPoint.m_z )
		m_oMinPoint.m_z = p.m_z;

	
	m_oDimension = vMax - m_oMinPoint;
	m_fBoundingSphereRadius = ComputeBoundingSphereRadius();
}

float CBox::ComputeBoundingSphereRadius() const
{
	CVector oCenter;
	GetCenter( oCenter );
	float fBoundingSphereRadius = 2 * ( m_oMinPoint - oCenter ).Norm();
	return fBoundingSphereRadius;
}

float CBox::ComputeBoundingCylinderRadius( TAxis eGeneratorAxis ) const
{
	CVector oCenter;
	float fRadius;
	switch( eGeneratorAxis )
	{
	case eAxisX:
		oCenter = m_oMinPoint + CVector( 0.f, m_oDimension.m_y / 2.f, m_oDimension.m_z / 2.f );
		break;
	case eAxisY:
		oCenter = m_oMinPoint + CVector( m_oDimension.m_x / 2.f, 0.f, m_oDimension.m_z / 2.f );
		break;
	case eAxisZ:
		oCenter = m_oMinPoint + CVector( m_oDimension.m_x / 2.f, m_oDimension.m_y / 2.f, 0.f );
		break;
	}
	fRadius = ( oCenter - m_oMinPoint ).Norm();
	return fRadius;
}

const CVector& CBox::GetBase() const
{
	return m_oMinPoint;
}

IGeometry* CBox::Duplicate()
{
	return new CBox(*this);
}

float CBox::GetHeight() const
{
	return m_oDimension.m_y;
}

void CBox::Transform(const CMatrix& tm)
{
	CVector oMaxPoint = m_oMinPoint + m_oDimension;
	m_oMinPoint = tm * m_oMinPoint;
	oMaxPoint = tm * oMaxPoint;
	m_oDimension = CVector(abs(oMaxPoint.m_x - m_oMinPoint.m_x), abs(oMaxPoint.m_y - m_oMinPoint.m_y), abs(oMaxPoint.m_z - m_oMinPoint.m_z));
}


bool CBox::TestBoxesCollisionIntoFirstBoxBase(const IBox& b1, const IBox& b2) const
{
	CMatrix b1Mat, b2Mat;
	b1.GetTM(b1Mat);
	b2.GetTM(b2Mat);
	CMatrix b1MatInv;
	b1Mat.GetInverse(b1MatInv);
	CMatrix b2MatBaseB1 = b1MatInv * b2Mat;

	CBox b2Temp;
	b2Temp.Set(b2.GetMinPoint(), b2.GetDimension());
	b2Temp.SetWorldMatrix(b2.GetTM());

	b2Temp.SetWorldMatrix(b2MatBaseB1);
	vector< CVector > vPoints2;
	b2Temp.GetPoints(vPoints2);
	float fMinx = CVector::GetMinx(vPoints2);
	if (fMinx > b1.GetMinPoint().m_x + b1.GetDimension().m_x)
		return false;
	float fMiny = CVector::GetMiny(vPoints2);
	if (fMiny > b1.GetMinPoint().m_y + b1.GetDimension().m_y)
		return false;
	float fMinz = CVector::GetMinz(vPoints2);
	if (fMinz > b1.GetMinPoint().m_z + b1.GetDimension().m_z)
		return false;
	float fMaxx = CVector::GetMaxx(vPoints2);
	if (fMaxx < b1.GetMinPoint().m_x)
		return false;
	float fMaxy = CVector::GetMaxy(vPoints2);
	if (fMaxy < b1.GetMinPoint().m_y)
		return false;
	float fMaxz = CVector::GetMaxz(vPoints2);
	if (fMaxz < b1.GetMinPoint().m_z)
		return false;
	return true;
}


float CBox::GetDistance(const IGeometry& oGeometry) const
{
	const CBox* pBox = dynamic_cast<const CBox*>(&oGeometry);
	if (pBox)
		return GetDistance(*pBox);
	return -1.f;
}

float CBox::GetDistance(const CBox& oBox) const
{
	float d1 = GetDistanceInBase(oBox);
	float d2 = ((CBox&)oBox).GetDistanceInBase(*this);
	return min(d1, d2);
}

float CBox::GetDistance(const ICylinder& oCylinder) const
{
	return -1.f;
}

float CBox::GetDistanceInBase(const IBox& oBox) const
{
	CMatrix b1Mat, b2Mat;
	GetTM(b1Mat);
	oBox.GetTM(b2Mat);
	CMatrix b1MatInv;
	b1Mat.GetInverse(b1MatInv);
	CMatrix b2MatBaseB1 = b1MatInv * b2Mat;

	CBox b2Temp;
	b2Temp.Set(oBox.GetMinPoint(), oBox.GetDimension());
	b2Temp.SetWorldMatrix(oBox.GetTM());

	b2Temp.SetWorldMatrix(b2MatBaseB1);
	vector< CVector > vPoints2;
	b2Temp.GetPoints(vPoints2);
	float fMinx = CVector::GetMinx(vPoints2);

	float distance = -1.f;
	if (fMinx > GetMinPoint().m_x + GetDimension().m_x)
		distance = fMinx - GetMinPoint().m_x + GetDimension().m_x;
	float fMiny = CVector::GetMiny(vPoints2);

	if (fMiny > GetMinPoint().m_y + GetDimension().m_y) {
		float d = fMiny - GetMinPoint().m_y + GetDimension().m_y;
		if (d < distance)
			distance = d;
	}

	float fMinz = CVector::GetMinz(vPoints2);
	if (fMinz > GetMinPoint().m_z + GetDimension().m_z) {
		float d = fMinz - GetMinPoint().m_z + GetDimension().m_z;
		if (d < distance)
			distance = d;
	}
		
	float fMaxx = CVector::GetMaxx(vPoints2);
	if (fMaxx < GetMinPoint().m_x){
		float d = fMaxx - GetMinPoint().m_x;
		if (d < distance)
			distance = d;
	}
	
	float fMaxy = CVector::GetMaxy(vPoints2);
	if (fMaxy < GetMinPoint().m_y) {
		float d = fMaxy - GetMinPoint().m_y;
		if (d < distance)
			distance = d;
	}
		
	float fMaxz = CVector::GetMaxz(vPoints2);
	if (fMaxz < GetMinPoint().m_z) {
		float d = fMaxz - GetMinPoint().m_z;
		if (d < distance)
			distance = d;
	}
		
	return distance;
}

float CBox::GetBoundingSphereRadius() const
{
	return m_fBoundingSphereRadius;
}

const IPersistantObject& CBox::operator >> (CBinaryFileStorage& store) const
{
	store << (int)eBox << m_oTM << m_oMinPoint << m_oDimension << m_fBoundingSphereRadius;
	return *this;
}

IPersistantObject& CBox::operator << (CBinaryFileStorage& store)
{
	store >> m_oTM >> m_oMinPoint >> m_oDimension >> m_fBoundingSphereRadius;
	return *this;
}

const IPersistantObject& CBox::operator >> (CAsciiFileStorage& store) const
{
	store << "Min point : " << m_oMinPoint << ", dimension : " << m_oDimension << ", radius : " << m_fBoundingSphereRadius << "\n";
	return *this;
}

IPersistantObject& CBox::operator << (CAsciiFileStorage& store)
{

	return *this;
}

const IPersistantObject& CBox::operator >> (CStringStorage& store) const
{
	return *this;
}

IPersistantObject& CBox::operator << (CStringStorage& store)
{
	return *this;
}

const CVector& CBox::GetMinPoint() const
{
	return m_oMinPoint;
}
	
void CBox::GetTM( CMatrix& m ) const
{
	m = m_oTM;
}

const CMatrix& CBox::GetTM() const
{
	return m_oTM;
}

void CBox::SetTM(const CMatrix& m)
{
	m_oTM = m;
}

const CVector& CBox::GetDimension() const
{
	return m_oDimension;
}

IBox& CBox::operator=( const IBox& oBox )
{
	m_bInitialized = true;
	m_fBoundingSphereRadius = oBox.GetBoundingSphereRadius();
	m_oMinPoint = oBox.GetMinPoint();
	oBox.GetTM(m_oTM);
	m_oDimension = oBox.GetDimension();
	return *this;
}

void CBox::SetWorldMatrix( const CMatrix& oMatrix )
{
	m_oTM = oMatrix;
}

void CBox::GetPoints( vector< CVector >& vPoints )
{
	vector< CVector > vTemp;
	vTemp.push_back( m_oMinPoint );
	vTemp.push_back( m_oMinPoint + CVector( m_oDimension.m_x, 0, 0 ) );
	vTemp.push_back( m_oMinPoint + CVector( 0, m_oDimension.m_y, 0 ) );
	vTemp.push_back( m_oMinPoint + CVector( 0, 0, m_oMinPoint.m_z ) );
	vTemp.push_back( m_oMinPoint + CVector( m_oDimension.m_x, m_oDimension.m_y, 0 ) );
	vTemp.push_back( m_oMinPoint + CVector( m_oDimension.m_x, 0, m_oDimension.m_z ) );
	vTemp.push_back( m_oMinPoint + CVector( 0, m_oDimension.m_y, m_oDimension.m_z ) );
	vTemp.push_back( m_oMinPoint + CVector( m_oDimension.m_x, m_oDimension.m_y, m_oDimension.m_z ) );
	for( int i = 0; i < vTemp.size(); i++ )
	{
		CVector v = m_oTM * vTemp[ i ];
		vPoints.push_back( v );
	}
}

bool CBox::IsIntersect(const CBox& box) const
{
	if (TestBoxesCollisionIntoFirstBoxBase(*this, box))
		return TestBoxesCollisionIntoFirstBoxBase(box, *this);
	return false;
}

bool CBox::IsIntersect(const IGeometry& box) const
{
	const CBox* pBox = dynamic_cast<const CBox*>(&box);
	if (pBox)
		return IsIntersect(*pBox);
	else {
		const CCylinder* pCylinder = dynamic_cast<const CCylinder*>(&box);
		if (pCylinder)
			return pCylinder->IsIntersect(*this);
	}
	return false;
}

void CBox::Draw(IRenderer& oRenderer) const
{
	oRenderer.DrawBox(m_oMinPoint, m_oDimension);
}

bool CBox::GetReactionYAlignedPlane(const ILine& oDirectriceLine, float planeHeight, CVector& R)
{
	CBox temp(*this);
	CMatrix m;
	temp.SetWorldMatrix(m);
	CMatrix oTMInv;
	m_oTM.GetInverse(oTMInv);
	CVector first, last;
	oDirectriceLine.GetPoints(first, last);
	CVector O = oTMInv * first;
	CVector P = oTMInv * last;
	bool collision = true;
		
	float dimx = m_oDimension.m_x / 2.f;
	float dimy = m_oDimension.m_y / 2.f;
	float dimz = m_oDimension.m_z / 2.f;

	if (!(dimy < O.m_y || -dimy > O.m_y + planeHeight)) {
		CVector A, B;
		if (O.m_x > dimx && P.m_x < dimx) {
			// collision sur le plan z positif
			A = CVector(dimx, O.m_y, -dimz);
			B = CVector(dimx, O.m_y, dimz);
		}
		else if (O.m_x < -dimx && P.m_x > -dimx) {
			// collision sur le plan z négatif
			A = CVector(-dimx, O.m_y, -dimz);
			B = CVector(-dimx, O.m_y, dimz);
		}
		else if (O.m_z > dimz && P.m_z < dimz) {
			// collision sur le plan x positif
			A = CVector(-dimx, O.m_y, dimz);
			B = CVector(dimx, O.m_y, dimz);
		}
		else if (O.m_z < -dimz && P.m_z > -dimz) {
			// collision sur le plan x négatif
			A = CVector(-dimx, O.m_y, -dimz);
			B = CVector(dimx, O.m_y, -dimz);
		}
		else
			collision = false;
		if (collision) {
			R = CVector(A.m_x, (O.m_y + P.m_y) / 2.f, P.m_z);
			R = m_oTM * R;
		}
	}
	return collision;
}