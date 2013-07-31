#include "math/Vector.h"

// stl
#include <string>

using namespace std;

class IRenderer;
class ILoaderManager;
class IEntity;
class IGUIManager;

class CRenderUtils
{
public:
	static void DrawBox( const CVector& oMinPoint, const CVector& oDimension, IRenderer& oRenderer  );
	static void ScreenCapture( string sFileName, IRenderer* pRenderer, ILoaderManager* pLoaderManager, IEntity* pScene, IGUIManager* pGUIManager = NULL );
};