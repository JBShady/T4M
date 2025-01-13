// ==========================================================
// T4M project
// 
// Component: clientdll
// Purpose: World at War related functions
//
// Initial author: UNKNOWN
// Started: 2015-07-08
// ==========================================================

#include "StdInc.h"
#include "T4.h"

// console commands
DWORD* cmd_id = (DWORD*)0x01F41670;
DWORD* cmd_argc = (DWORD*)0x01F416B4;
DWORD** cmd_argv = (DWORD**)0x01F416D4;


extern "C"
{
	AddFunction_t AddFunction = (AddFunction_t)0x00682040;

	Com_Error_t Com_Error = (Com_Error_t)0x006C1CE0;
	Com_Printf_t Com_Printf = (Com_Printf_t)0x0059A2C0;
	Com_PrintMessage_t Com_PrintMessage = (Com_PrintMessage_t)0x59A170;

	CScr_GetFunction_t CScr_GetFunction = (CScr_GetFunction_t)0x0066EA30;
	CScr_GetMethod_t CScr_GetMethod = (CScr_GetMethod_t)0x00671110;

	DB_EnumXAssets_t DB_EnumXAssets = (DB_EnumXAssets_t)0x006DA180;
	DB_EnumXAssets_FastFile_t DB_EnumXAssets_FastFile = (DB_EnumXAssets_FastFile_t)0x0048DEA0;
	DB_FindXAssetHeader_t DB_FindXAssetHeader = (DB_FindXAssetHeader_t)0x48DA30;
	DB_LoadXAssets_t DB_LoadXAssets = (DB_LoadXAssets_t)0x48E7B0;

	EmitMethod_t EmitMethod = (EmitMethod_t)0x00682F40;

	RemoveRefToValue_t RemoveRefToValue = (RemoveRefToValue_t)0x00690130;

	CL_CGameRendering_t CL_CGameRendering = (CL_CGameRendering_t)0x00478D20;

	Scr_GetFunction_t Scr_GetFunction = (Scr_GetFunction_t)0x0052F0B0;
	//Scr_GetMethod_t Scr_GetMethod = (Scr_GetMethod_t)0x00530630;

	// GetMethod stuff
	Player_GetMethod_t Player_GetMethod = (Player_GetMethod_t)0x004DEEA0;
	ScriptEnt_GetMethod_t ScriptEnt_GetMethod = (ScriptEnt_GetMethod_t)0x00567680;
	ScriptVehicle_GetMethod_t ScriptVehicle_GetMethod = (ScriptVehicle_GetMethod_t)0x004F3920;
	HudElem_GetMethod_t HudElem_GetMethod = (HudElem_GetMethod_t)0x00532BA0;
	Helicopter_GetMethod_t Helicopter_GetMethod = (Helicopter_GetMethod_t)0x00541F80;
	Actor_GetMethod_t Actor_GetMethod = (Actor_GetMethod_t)0x004FC320;
	BuiltIn_GetMethod_t BuiltIn_GetMethod = (BuiltIn_GetMethod_t)0x005305B0;

	XAssetEntryPoolEntry* g_assetEntryPool = (XAssetEntryPoolEntry *)0x00A51C50;
	XZoneName* g_zoneNames = (XZoneName *)0x00D04CB0;
	unsigned __int16 * db_hashTable = (unsigned __int16 *)0x00987088;

	DB_XAssetGetNameHandler * DB_XAssetGetNameHandlers = (DB_XAssetGetNameHandler *)0x008DCAF8;
}

// fucking __usercall
dvar_t* Dvar_RegisterBool(bool value, const char *dvarName, int flags, const char *description)
{
	DWORD func = 0x5EEE20;
	dvar_t* ret;
	__asm
	{
		push description
		push flags
		mov al, value
		mov edi, dvarName
		call func
		add esp, 8
		mov ret, eax
	}
	return ret;
}

//typedef dvar_t* (__fastcall* DvarRegisterFloatFunc)(const char* dvarName, float defaultValue, float min, float max, int flags, const char* description);
//DvarRegisterFloatFunc Dvar_RegisterFloat = (DvarRegisterFloatFunc)0x5EEF10;
// fucking __usercall -- Clippy95 ~10 years later.
dvar_t* Dvar_RegisterFloat(const char* dvarName, float defaultValue, float min, float max, int flags, const char* description)
{
	DWORD func = 0x5EEF10;
	dvar_t* ret;

	__asm
	{
		movss xmm0, defaultValue
		push description
		push flags
		sub esp, 8
		fld max
		fstp dword ptr[esp + 4]
		fld min
		fstp dword ptr[esp]
		mov edi, dvarName
		call func
		add esp, 0x10
		mov ret, eax
	}

	return ret;
}

//int __usercall CBuf_AddText@<eax>(int a1@<eax>, int a2@<ecx>), a1 = text, a2 = localClientNum
void Cbuf_AddText(const char* text, int localClientNum)
{
	DWORD func = 0x00594200;

	__asm
	{
		mov eax, text
		mov ecx, localClientNum
		call func
	}
}

/*
============
Cmd_AddCommand
============
*/
cmd_function_s** cmd_functions = ((cmd_function_s**)(cmd_functions_ADDR));
void Cmd_AddCommand(const char *cmd_name, xcommand_t function) {
	cmd_function_s *cmd;

	// fail if the command already exists
	for (cmd = *cmd_functions; cmd; cmd = cmd->next) {
		if (!strcmp(cmd_name, cmd->name)) {
			// allow completion-only commands to be silently doubled
			if (function != NULL) {
				Com_Printf(0, "Cmd_AddCommand: %s already defined\n", cmd_name);
			}
			return;
		}
	}

	// use a small malloc to avoid zone fragmentation
	cmd = (cmd_function_s *)malloc(sizeof(cmd_function_s) + strlen(cmd_name) + 1);
	strcpy((char*)(cmd + 1), cmd_name);
	cmd->name = (char*)(cmd + 1);
	cmd->autocomplete1 = NULL;
	cmd->autocomplete2 = NULL;
	cmd->function = function;
	cmd->next = *cmd_functions;
	*cmd_functions = cmd;
}

typedef int(__cdecl * DB_GetXAssetSizeHandler_t)();
DB_GetXAssetSizeHandler_t* DB_GetXAssetSizeHandler = (DB_GetXAssetSizeHandler_t*)0x008DCC18;

void** DB_XAssetPool = (void**)0x8DC828;
unsigned int* g_poolSize = (unsigned int*)0x8DC5D0;

void* DB_ReallocXAssetPool(XAssetType type, unsigned int size)
{
	int elSize = DB_GetXAssetTypeSize(type);
	void* poolEntry = malloc(size * elSize);
	DB_XAssetPool[type] = poolEntry;
	g_poolSize[type] = size;
	return poolEntry;
}

void __cdecl DB_ListAssetPool(XAssetType type, bool count_only)
{
	char *v1;
	XZoneName *v2;
	const char *v3;
	XAssetEntry *overrideAssetEntry;
	XZoneName *v5;
	const char *v6;
	char *v7;
	unsigned int nextAssetEntryIndex;
	unsigned int hash;
	unsigned int assetPoolSize;
	unsigned int assetEntryIndex;
	unsigned int assetPoolCount;
	XAssetEntry *assetEntry;
	unsigned int overrideAssetEntryIndex;
	assetPoolCount = 0;
	assetPoolSize = 0;
	v1 = DB_GetXAssetTypeName(type);
	
	if (!count_only)
		Com_Printf(0, "Listing assets in %s pool.\n", v1);

	for (hash = 0; hash < 0x8000; ++hash)
	{
		for (assetEntryIndex = db_hashTable[hash]; assetEntryIndex; assetEntryIndex = nextAssetEntryIndex)
		{
			assetEntry = &g_assetEntryPool[assetEntryIndex].entry;
			nextAssetEntryIndex = g_assetEntryPool[assetEntryIndex].entry.nextHash;
			if (g_assetEntryPool[assetEntryIndex].entry.asset.type == type)
			{
				++assetPoolCount;
				v2 = &g_zoneNames[assetEntry->zoneIndex];
				v3 = DB_GetXAssetName(&assetEntry->asset);
				if (!count_only)
					Com_Printf(0, "Asset: %s FF: %s\n", v3, v2->name);
				assetPoolSize += DB_GetXAssetTypeSize(assetEntry->asset.type);
				for (overrideAssetEntryIndex = assetEntry->nextOverride;
					overrideAssetEntryIndex;
					overrideAssetEntryIndex = overrideAssetEntry->nextOverride)
				{
					overrideAssetEntry = &g_assetEntryPool[overrideAssetEntryIndex].entry;
					++assetPoolCount;
					v5 = &g_zoneNames[g_assetEntryPool[overrideAssetEntryIndex].entry.zoneIndex];
					v6 = DB_GetXAssetName(&g_assetEntryPool[overrideAssetEntryIndex].entry.asset);
					if (!count_only)
						Com_Printf(0, "Asset: %s FF: %s | overriden\n", v6, v5);
					assetPoolSize += DB_GetXAssetTypeSize(overrideAssetEntry->asset.type);
				}
			}
		}
	}
	v7 = DB_GetXAssetTypeName(type);
	Com_Printf(16, "Total of %d assets in %s pool, max %d, size %d\n", assetPoolCount, v7, g_poolSize[type], assetPoolSize);
}

char *__cdecl DB_GetXAssetTypeName(int type)
{
	char** g_assetNames = (char **)0x008DCA68;
	return g_assetNames[type];
}

const char *__cdecl DB_GetXAssetName(XAsset *asset)
{
	return DB_GetXAssetHeaderName(asset->type, &asset->header);
}

const char *__cdecl DB_GetXAssetHeaderName(int type, XAssetHeader *header)
{
	int v2;
	const char *name;
	name = DB_XAssetGetNameHandlers[type](header);
	return name;
}

void __cdecl Scr_GetInt(scriptInstance_t inst, unsigned int index)
{
	static DWORD func = 0x00699C50;
	__asm
	{
		push inst
		push index
		call func
	}
}

int __cdecl DB_GetXAssetTypeSize(int type)
{
	return DB_GetXAssetSizeHandler[type]();
}