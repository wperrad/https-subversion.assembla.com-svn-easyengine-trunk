#ifndef NODE_H
#define NODE_H

#pragma warning(disable:4251)

// stl
#include <vector>
#include <string>

#include "EntityAPI.h"
#include "../../../Utils/math/matrix.h"


class ENTITY_API CNode
{

protected:
	CNode*                     			m_pParent;
	std::vector< CNode* >      			m_vChild;
	//CMatrix								m_oLocalMatrix;
	//CMatrix								m_oWorldMatrix;
	CMatrix								m_oTransformMatrix;
	CMatrix								m_oGlobalMatrix;
	std::string							m_sName;
	int									m_nID;
	int									m_nParentID;
	unsigned int						m_nProgram;


public:
	                    				CNode();
	virtual             				~CNode();
	unsigned int        				GetChildCount() const;
	CNode*              				GetChild( unsigned int nIdx ) const;
  	virtual void	    			    Update();
  	void                				Link(CNode* pNode);
  	void                				AddChild(CNode* pNode);
  	void                				SetParent(CNode* pNode);
  	virtual void                		Yaw(float fAngle);
  	virtual void                		Pitch(float fAngle);
  	virtual void                		Roll(float fAngle);
  	virtual void                		LocalTranslate(float dx , float dy , float dz);
  	void                				SetWorldPosition(float x, float y , float z);
	virtual CVector                		GetWorldPosition() const;
	virtual void						SetTransformMatrix( const CMatrix& oMat );
	virtual void						GetTransformMatrix( CMatrix& oMat );
	void								SetName( const std::string& sName );
	void								GetName( std::string& sName ) const;
};

#endif  //NODE_H