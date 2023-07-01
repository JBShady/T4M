// --------------------------------------+
// System Dynamic Link Library Proxy
// by momo5502
// --------------------------------------+

#include "StdInc.h"

// Macro to declare an export
// --------------------------------------+
#define EXPORT(_export) extern "C" __declspec(naked) __declspec(dllexport) void _export() { static FARPROC function = 0; if(!function) function = SDLLP::GetExport(__FUNCTION__, LIBRARY); __asm { jmp function } }  

// Static class
// --------------------------------------+
class SDLLP
{
private:
    static std::map<std::string, HINSTANCE> mLibraries;

    static void	Log(const char* message, ...);
    static void	LoadLibrary(const char* library);
    static bool	IsLibraryLoaded(const char* library);

public:
    static FARPROC GetExport(const char* function, const char* library);
};

//	Class variable declarations
// --------------------------------------+
std::map<std::string, HINSTANCE> SDLLP::mLibraries;

// Load necessary library
// --------------------------------------+
void SDLLP::LoadLibrary(const char* library)
{
    Log("[SDLLP] Loading library '%s'.", library);

    CHAR mPath[MAX_PATH];
    CHAR d3d9Path[MAX_PATH];

    GetModuleFileNameA(NULL, mPath, MAX_PATH);
    PathRemoveFileSpecA(mPath);
    PathCombineA(d3d9Path, mPath, "T4Me\\d3d9.dll");

    if (PathFileExistsA(d3d9Path))
    {
        Log("[SDLLP] Found d3d9.dll in T4Me subdirectory.");
        mLibraries[library] = ::LoadLibraryA(d3d9Path);
    }
    else
    {
        GetSystemDirectoryA(mPath, MAX_PATH);
        strcat_s(mPath, "\\");
        strcat_s(mPath, library);

        mLibraries[library] = ::LoadLibraryA(mPath);
    }

    if (!IsLibraryLoaded(library)) Log("[SDLLP] Unable to load library '%s'.", library);
}


// Check if export already loaded
// --------------------------------------+
bool SDLLP::IsLibraryLoaded(const char* library)
{
    return (mLibraries.find(library) != mLibraries.end() && mLibraries[library]);
}

// Get export address
// --------------------------------------+
FARPROC SDLLP::GetExport(const char* function, const char* library)
{
    Log("[SDLLP] Export '%s' requested from %s.", function, library);

    if (!IsLibraryLoaded(library)) LoadLibrary(library);

    FARPROC address = GetProcAddress(mLibraries[library], function);

    if (!address) Log("[SDLLP] Unable to load export '%s' from library '%s'.", function, library);
    return address;
}

// Write debug string
// --------------------------------------+
void SDLLP::Log(const char* message, ...)
{
    CHAR buffer[1024];
    va_list ap;

    va_start(ap, message);
    vsprintf(buffer, message, ap);
    va_end(ap);

    OutputDebugStringA(buffer);
}

// --------------------------------------+
//	Adapt export functions and library
// --------------------------------------+

#define LIBRARY "d3d9.dll"
EXPORT(D3DPERF_BeginEvent)
EXPORT(D3DPERF_EndEvent)
EXPORT(Direct3DCreate9)