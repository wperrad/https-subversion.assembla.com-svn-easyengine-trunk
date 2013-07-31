
#ifdef CORE2_EXPORTS
#define CORE2_API __declspec(dllexport)
#else
#define CORE2_API __declspec(dllimport)
#endif