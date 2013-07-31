#include "IGeometry.h"
#include <map>


using namespace std;

class CWeightTable : public IWeightTable
{
public:
	map< int, map< int, float > >	m_mVertex;
	void	Add( int iVertexIndex, int nBoneID, float fBoneWeight );
	bool	Get( int iVertexIndex, std::map< int, float >& mWeight ) const;
	int		GetVertexCount() const;
	void	GetArrays( vector<float >& vWeightVertex, vector< float >& vWeigtedVertexID );
	void	BuildFromArrays( const vector<float >& vWeightVertex, const vector< float >& vWeightedVertexID );

};