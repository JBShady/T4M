#include "StdInc.h"

ScreenPlacement* scrPlace = reinterpret_cast<ScreenPlacement*>(0x8F8C68);
ScreenPlacement* scrPlaceFull = reinterpret_cast<ScreenPlacement*>(0x8F8C20);
ScreenPlacement* scrPlaceFullUnsafe = reinterpret_cast<ScreenPlacement*>(0x8F8CB0);
float ScrPlace_SetupViewport(ScreenPlacement* scrPlace, float viewportX, float viewportY, float viewportWidth, float viewportHeight, BYTE inmenu = 0);
int mainloop() {
	BYTE* isMenu = (BYTE*)(0x0208F194);
	float viewportWidth = (float)*(int*)0x03BED830;
	float viewportHeight = (float)*(int*)0x03BED834;
	ScrPlace_SetupViewport(scrPlace, 0.0f, 0.0f, viewportWidth, viewportHeight, *isMenu);
	ScrPlace_SetupViewport(scrPlaceFull, 0.0f, 0.0f, viewportWidth, viewportHeight, *isMenu);
	ScrPlace_SetupViewport(scrPlaceFullUnsafe, 0.0f, 0.0f, viewportWidth, viewportHeight, *isMenu);
	return CL_CGameRendering();
}

void PatchT4E_CGameRendering() {

	Detours::X86::DetourFunction((PBYTE)0x0047924B, (PBYTE)&mainloop, Detours::X86Option::USE_CALL);
}