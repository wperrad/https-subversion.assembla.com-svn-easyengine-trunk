#ifdef OGLAPI_EXPORTS
#define OGLAPI_API __declspec(dllexport)
#else
#define OGLAPI_API __declspec(dllimport)
#endif


class OGLAPI_API COGLAPI 
{
public:
	struct GLPARAM
	{
		void(*OnPaint)();
		void(*OnUpdate)();
		int m_nWidth;
		int m_nHeight;
		int m_nBits;
		bool m_bFullscreen;
	};
	int		CreateOGLWindow( const GLPARAM& param );
	void	Display( int nResID );
	void	KillWindow( int nResID ) const;
	COGLAPI(void);
};
