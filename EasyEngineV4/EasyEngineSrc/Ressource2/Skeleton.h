#ifndef SKELETON_CPP
#ifndef RESSOURCEMANAGER_CPP
#error
#endif
#endif

#ifndef SKELETON_H
#define SKELETON_H

class CNode;

class CSkeleton
{
	CNode*					_pRoot;
	unsigned int			_nCurrentID;
public:
							CSkeleton(void);
	virtual					~CSkeleton(void);
	void					AddNewHierarchy(CNode* pRoot);

	
};



#endif //SKELETON_H