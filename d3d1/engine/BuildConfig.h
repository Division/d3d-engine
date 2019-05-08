#if defined ENV_PRODUCTION
#define ENGINE_DIRECTX_DEBUG false
#define CRASHDUMP_ENABLED true
#else
#define ENGINE_DIRECTX_DEBUG true
#define CRASHDUMP_ENABLED false
#endif

// It's being auto incremented by the pre build script
#define BUILD_COMMIT_HASH L"0e7f820"

