#ifndef NODE_H
#define NODE_H


// stl
#include <vector>
#include <string>

// Engine
#include "math/matrix.h"
#include "math/Quaternion.h"

using namespace std;

class CNode
{

protected:
	CNode*                     	m_pParent;
	std::vector< CNode* >      	m_vChild;
	CMatrix						m_oLocalMatrix;
	CMatrix						m_oWorldMatrix;
	CQuaternion					m_oLocalQuaternion;
	CQuaternion					m_oWorldQuaternion;
	string						m_sName;
	int							m_nID;
	int							m_nParentID;
	bool						m_bQuaternionMode;
	CVector						m_vConstantLocalRotate;
	CVector						m_vConstantLocalTranslate;
	bool						m_bUpdateConstantLocalTranslate;
	bool						m_bUpdateConstantLocalRotate;


public:
	                    		CNode();
	virtual             		~CNode();
	unsigned int        		GetChildCount() const;
	CNode*              		GetChild( unsigned int nIdx ) const;
  	virtual void	    		Update();
	virtual void	    		UpdateWithoutChildren();
  	virtual void                Link( CNode* pNode );
	virtual void				Unlink();
  	void                		AddChild( CNode* pNode );
  	void                		SetParent( CNode* pNode );
  	virtual void                Yaw(float fAngle);
  	virtual void                Pitch(float fAngle);
  	virtual void                Roll(float fAngle);
  	virtual void                LocalTranslate(float dx , float dy , float dz);
	virtual void                LocalTranslate( const CVector& vTranslate );
	virtual void				WorldTranslate( float dx , float dy , float dz );
	virtual void				WorldTranslate( const CVector& vTranslate );
  	void                		SetLocalPosition(float x, float y , float z);
	void                		SetLocalPosition( const CVector& vPos );
	void                		SetWorldPosition(float x, float y, float z);
	void                		SetWorldPosition(const CVector& vPos);
	virtual void				GetWorldPosition( CVector& vPosition ) const;
	virtual void				SetLocalMatrix( const CMatrix& oMat );
	virtual void				SetWorldMatrix( const CMatrix& oMat );
	virtual void				GetLocalQuaternion( CQuaternion& oQuaternion );
	virtual void				SetLocalQuaternion( const CQuaternion& oQuaternion );
	virtual void				GetLocalMatrix( CMatrix& oMat ) const;
	virtual void				SetName( const std::string& sName );
	virtual void				GetName( std::string& sName ) const;
	virtual CNode*				DuplicateHierarchy();
	void						SetID( int nID );
	int							GetID() const;
	void						GetWorldMatrix( CMatrix& oMatrix );
	const CMatrix&				GetWorldMatrix() const;
	void						GetWorldQuaternion( CQuaternion& oQuaternion );
	void						GetHierarchyCount( int& nCount );
	void						SetQuaternionMode( bool bQuarternion );
	CNode*						GetParent();
	virtual CNode*				GetChildBoneByID( int nID );
	virtual CNode*				GetChildBoneByName( string sName );
	virtual void				SetLocalTMByWorldTM();
	void						ClearChilds();
	virtual void				UpdateTime( float fTime );
	virtual void				ConstantLocalRotate( const CVector& vRotation );
	virtual void				ConstantLocalTranslate( const CVector& vTranslate );
	virtual float				GetDistance( CNode* pNode );
	float						GetX();
	float						GetY();
	float						GetZ();
};

#endif  //NODE_H