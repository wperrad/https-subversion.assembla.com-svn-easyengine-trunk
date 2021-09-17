#include "BBoxLoader.h"
#include "IGeometry.h"
#include "IFileSystem.h"

CBBoxLoader::CBBoxLoader(IFileSystem& oFileSystem, IGeometryManager& oGeometryManager) :
	m_oFileSystem(oFileSystem),
	m_oGeometryManager(oGeometryManager)
{
}


CBBoxLoader::~CBBoxLoader()
{
}

void CBBoxLoader::Load(string sFileName, ILoader::IRessourceInfos& ri, IFileSystem&)
{
	CAnimationBBoxInfos& bboxInfos = dynamic_cast<CAnimationBBoxInfos&>(ri);
	string sFolder;
	m_oFileSystem.GetLastDirectory(sFolder);
	CBinaryFileStorage fs;
	CMeshInfos mi;
	string sFilePath = sFolder.empty() ? sFileName : (sFolder + "\\" + sFileName);
	if (fs.OpenFile(sFilePath, CBinaryFileStorage::eRead)) {
		map<int, IBox*> mBoxes;
		LoadKeyBoundingBoxes(fs, bboxInfos.mKeyBoundingBoxes);
		fs.CloseFile();
	}
	else {
		CFileNotFoundException e(sFilePath);
		throw e;
	}
}

void CBBoxLoader::LoadKeyBoundingBoxes(CBinaryFileStorage& fs, map< string, map< int, IBox* > >& mKeyBoundingBoxes)
{
	int nAnimationCount;
	fs >> nAnimationCount;
	for (int i = 0; i < nAnimationCount; i++)
	{
		string sAnimationName;
		fs >> sAnimationName;
		int nBoxCount;
		fs >> nBoxCount;
		for (int i = 0; i < nBoxCount; i++)
		{
			int iKeyIndex;
			fs >> iKeyIndex;
			int type = 0;
			fs >> type;
			IBox* pBox = m_oGeometryManager.CreateBox();
			fs >> *pBox;
			mKeyBoundingBoxes[sAnimationName][iKeyIndex] = pBox;
		}
	}
}

void CBBoxLoader::Export(string sFileName, ILoader::IRessourceInfos& ri)
{
	try
	{
		ILoader::CAnimationBBoxInfos bboxInfos;
		try {

			Load(sFileName, bboxInfos, m_oFileSystem);
		}
		catch (CFileNotFoundException& e) {

		}
		CAnimationBBoxInfos& newBBoxInfos = static_cast<CAnimationBBoxInfos&>(ri);
		UpdateBBoxInfos(bboxInfos, newBBoxInfos);

		CBinaryFileStorage fs;
		if (!fs.OpenFile(sFileName, IFileStorage::eWrite))
		{
			CFileNotFoundException e(sFileName);
			throw e;
		}
		
		ExportKeyBoundingBoxesInfos(bboxInfos.mKeyBoundingBoxes, fs);
		
	}
	catch (exception& e)
	{
		MessageBoxA(NULL, e.what(), "Erreur", MB_ICONERROR);
	}
}

void CBBoxLoader::UpdateBBoxInfos(CAnimationBBoxInfos& oldBBox, const CAnimationBBoxInfos& newBBox)
{
	for (map< string, map< int, IBox* > >::const_iterator itAnim = newBBox.mKeyBoundingBoxes.begin(); itAnim != newBBox.mKeyBoundingBoxes.end(); itAnim++) {		
		map< string, map< int, IBox* > >::const_iterator itOldAnim = oldBBox.mKeyBoundingBoxes.find(itAnim->first);
		if (itOldAnim != oldBBox.mKeyBoundingBoxes.end()) {
			itOldAnim = oldBBox.mKeyBoundingBoxes.erase(itOldAnim);
		}
		for (map< int, IBox* >::const_iterator itBoxes = itAnim->second.begin(); itBoxes != itAnim->second.end(); itBoxes++) {
			oldBBox.mKeyBoundingBoxes[itAnim->first][itBoxes->first] = itBoxes->second;
		}
	}
}

void CBBoxLoader::ExportKeyBoundingBoxesInfos(const map< string, map< int, IBox* > >& mKeyBoundingBoxes, CBinaryFileStorage& fs)
{
	int nAnimationCount = (int)mKeyBoundingBoxes.size();
	fs << nAnimationCount;
	if (nAnimationCount > 0)
	{
		for (map< string, map< int, IBox* > >::const_iterator itAnim = mKeyBoundingBoxes.begin(); itAnim != mKeyBoundingBoxes.end(); itAnim++)
		{
			fs << itAnim->first;
			const map< int, IBox* >& oKeyBoxes = itAnim->second;
			fs << (unsigned int)oKeyBoxes.size();
			for (map< int, IBox* >::const_iterator itBox = oKeyBoxes.begin(); itBox != oKeyBoxes.end(); itBox++)
			{
				fs << itBox->first;
				fs << *itBox->second;
			}
		}
	}
}

