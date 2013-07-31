#ifndef INODE_H
#define INODE_H

#include "math/matrix.h"

// stl
#include <string>


class INode
{
public:
	unsigned int        				GetChildCount() const;
	INode*              				GetChild( unsigned int nIdx ) const;
  	virtual void	    			    Update();
  	void                				Link( INode* pNode );
  	void                				AddChild( INode* pNode );
  	void                				SetParent( INode* pNode );
  	virtual void                		Yaw(float fAngle);
  	virtual void                		Pitch(float fAngle);
  	virtual void                		Roll(float fAngle);
  	virtual void                		LocalTranslate(float dx , float dy , float dz);
  	void                				SetWorldPosition(float x, float y , float z);
	virtual CVector                		GetWorldPosition() const;
	virtual void						SetLocalMatrix( const CMatrix& oMat );
	virtual void						GetLocalMatrix( CMatrix& oMat ) const;
	void								SetName( const std::string& sName );
	void								GetName( std::string& sName ) const;
	INode*								DuplicateHierarchy();
	void								SetID( int nID );
	int									GetID() const;
	void								GetWorldMatrix( CMatrix& oMatrix );
	void								GetHierarchyCount( int& nCount );
};

#endif // INODE_H