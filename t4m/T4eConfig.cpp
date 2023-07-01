#include "StdInc.h"
dvar_t* r_dxvk;
void CreateConfigFile()
{
    r_dxvk = Dvar_RegisterBool(0, "r_dxvk", DVAR_FLAG_ARCHIVE, "Load d3d9.dll from the T4ME folder instead of System, used for DXVK which translates DX9 to Vulkan, might improve perfomance.)");
    CHAR mPath[MAX_PATH];
    CHAR t4mePath[MAX_PATH];
    CHAR iniPath[MAX_PATH];

    GetModuleFileNameA(NULL, mPath, MAX_PATH);
    PathRemoveFileSpecA(mPath);
    PathCombineA(t4mePath, mPath, "T4Me");
    PathCombineA(iniPath, t4mePath, "T4Me.ini");

    if (!PathFileExistsA(t4mePath))
    {
        CreateDirectoryA(t4mePath, NULL);
    }

    if (!PathFileExistsA(iniPath))
    {
        WritePrivateProfileStringA("Settings", "dxvk", "0", iniPath);
    }
}
void SetDxvkFlag(bool value)
{
    CHAR mPath[MAX_PATH];
    CHAR t4mePath[MAX_PATH];
    CHAR iniPath[MAX_PATH];

    GetModuleFileNameA(NULL, mPath, MAX_PATH);
    PathRemoveFileSpecA(mPath);
    PathCombineA(t4mePath, mPath, "T4Me");
    PathCombineA(iniPath, t4mePath, "T4Me.ini");

    WritePrivateProfileStringA("Settings", "dxvk", value ? "1" : "0", iniPath);
}
bool GetDxvkFlag()
{
    CHAR mPath[MAX_PATH];
    CHAR t4mePath[MAX_PATH];
    CHAR iniPath[MAX_PATH];
    CHAR buffer[10];

    GetModuleFileNameA(NULL, mPath, MAX_PATH);
    PathRemoveFileSpecA(mPath);
    PathCombineA(t4mePath, mPath, "T4Me");
    PathCombineA(iniPath, t4mePath, "T4Me.ini");

    GetPrivateProfileStringA("Settings", "dxvk", "0", buffer, 10, iniPath);

    return (strcmp(buffer, "1") == 0);
}
void dvarset()
{
    if (r_dxvk->current.value)
    {
        SetDxvkFlag(true);
    }
    else
    {
        SetDxvkFlag(false);
    }

}
