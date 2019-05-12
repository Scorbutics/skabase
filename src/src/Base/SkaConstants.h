#ifndef NDEBUG
#define SKA_DEBUG_GRAPHIC
#define SKA_DEBUG_LOGS
#endif

#if defined(__linux__) || defined(__unix__) || defined(linux) || defined(__linux)
#define SKA_PLATFORM_LINUX
#elif defined(_WIN32) || defined(_WIN64)
#define SKA_PLATFORM_WIN
#else
#define SKA_PLATFORM_UNKNOWN
#endif

#if defined(__cpp_exceptions) && __cpp_exceptions < 199711L 
#define SKA_EXCEPTIONS_DISABLED
#endif
