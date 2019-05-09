#if defined ENV_PRODUCTION
#define ENGINE_DIRECTX_DEBUG false
#define CRASHDUMP_ENABLED true
#else
#define ENGINE_DIRECTX_DEBUG true
#define CRASHDUMP_ENABLED false
#endif

// It's being auto updated by the pre-build script (Production builds only)
// Used to generate crash dump name 
#define BUILD_COMMIT_HASH L"87a9500"

