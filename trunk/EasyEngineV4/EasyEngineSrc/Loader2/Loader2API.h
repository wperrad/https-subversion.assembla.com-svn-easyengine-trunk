#ifdef LOADER2_EXPORTS
#define LOADER2_API __declspec(dllexport)
#else
#define LOADER2_API __declspec(dllimport)
#endif

