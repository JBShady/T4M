#include "StdInc.h"
dvar_t* safeArea_horizontal;
dvar_t* safeArea_vertical;

struct ScreenPlacement
{
	float scaleVirtualToReal[2];
	float scaleVirtualToFull[2];
	float scaleRealToVirtual[2];
	float virtualViewableMin[2];
	float virtualViewableMax[2];
	float realViewportSize[2];
	float realViewableMin[2];
	float realViewableMax[2];
	float subScreen[2];
};


float ScrPlace_CalcSafeAreaOffsets(float* realViewableMin, float* realViewableMax, float viewportX, float viewportY, float viewportWidth, float viewportHeight, float aspect, float safeAreaRatioHorz, float safeAreaRatioVert, float* virtualViewableMin, float* virtualViewableMax, int width, int height)
{
	double v15;
	double v16;
	double v17;
	double v18;
	double v19;
	double v20;
	float v21;
	float v22;
	float v23;
	float v24;
	float v25;
	float v26;
	float v27;
	float v28;
	float a10a;
	float a11c;
	float a11a;
	float a11b;
	float a12b;
	float a12c;
	float a12d;
	int a12a;

	a11c = (float)width;
	v21 = (1.0 - safeAreaRatioHorz) * 0.5 * a11c;
	a10a = (float)height;
	v23 = (1.0 - safeAreaRatioVert) * 0.5 * a10a;
	a12b = v21 + 0.5;
	a12c = floor(a12b);
	v22 = a12c;
	a12d = v23 + 0.5;
	*(float*)&a12a = floor(a12d);
	v27 = a11c - v22;
	v28 = a10a - *(float*)&a12a;
	if (viewportX >= (double)v22)
		v22 = viewportX;
	if (viewportY < (double)*(float*)&a12a)
	{
		v15 = viewportY;
		v16 = viewportX;
		v24 = *(float*)&a12a;
	}
	else
	{
		v15 = viewportY;
		v16 = viewportX;
		v24 = viewportY;
	}
	v17 = viewportWidth;
	a11a = v16 + viewportWidth;
	v18 = v27;
	if (a11a <= (double)v27)
		v18 = a11a;
	v25 = v18;
	v19 = viewportHeight;
	a11b = v15 + viewportHeight;
	v20 = v28;
	if (a11b <= (double)v28)
		v20 = a11b;
	v26 = v20;
	*realViewableMin = v22 - v16;
	realViewableMin[1] = v24 - v15;
	*realViewableMax = v25 - v16;
	realViewableMax[1] = v26 - v15;
	*virtualViewableMin = *realViewableMin * aspect * (640.0 / v17);
	virtualViewableMin[1] = realViewableMin[1] * (480.0 / v19);
	*virtualViewableMax = 640.0 / v17 * (aspect * *realViewableMax);
	virtualViewableMax[1] = 480.0 / v19 * realViewableMax[1];

	return virtualViewableMin[1];
}

float ScrPlace_SetupViewport(ScreenPlacement *scrPlace, float viewportX, float viewportY, float viewportWidth, float viewportHeight)
{

	float vidConfig_aspectRatioScenePixel = *(float*)0x04DA90D4;

	viewportX = 0.0f;
	viewportY = 0.0f;

	viewportWidth = (float)*(int*)0x03BED830;
	viewportHeight = (float)*(int*)0x03BED834;

	vidConfig_aspectRatioScenePixel = 1.f;
	scrPlace->realViewportSize[0] = viewportWidth;
	scrPlace->realViewportSize[1] = viewportHeight;
	float adjustedRealWidth = 1.333333373069763 * viewportHeight / vidConfig_aspectRatioScenePixel;
	if (adjustedRealWidth > viewportWidth)
		adjustedRealWidth = viewportWidth;

	float result = ScrPlace_CalcSafeAreaOffsets(
		scrPlace->realViewableMin,
		scrPlace->realViewableMax,
		viewportX,
		viewportY,
		viewportWidth,
		viewportHeight,
		viewportWidth / adjustedRealWidth,
		safeArea_horizontal->current.value,
		safeArea_vertical->current.value,
		scrPlace->virtualViewableMin,
		scrPlace->virtualViewableMax,
		viewportWidth,
		viewportHeight
	);

	scrPlace->scaleVirtualToReal[0] = adjustedRealWidth / 640.0;
	scrPlace->scaleVirtualToReal[1] = viewportHeight / 480.0;
	scrPlace->scaleVirtualToFull[0] = viewportWidth / 640.0;
	scrPlace->scaleVirtualToFull[1] = viewportHeight / 480.0;
	scrPlace->scaleRealToVirtual[0] = 640.0 / adjustedRealWidth;
	scrPlace->scaleRealToVirtual[1] = 480.0 / viewportHeight;
	scrPlace->subScreen[0] = (viewportWidth - adjustedRealWidth) * 0.5;
	scrPlace->subScreen[1] = 0.f;
	return result;
}

__declspec(naked) void ScrPlace_SetupFloatViewportDetour() {
	__asm {
s
		pushad
		pushfd

		mov ecx, edi


		push[esp + 0x24 + 0x20]
		push[esp + 0x24 + 0x1C]
		push[esp + 0x24 + 0x18]
		push[esp + 0x24 + 0x14]
		push ecx

		call ScrPlace_SetupViewport

		add esp, 20

		popfd
		popad

		ret
	}
}

void PatchT4E_Window() {
	safeArea_horizontal = Dvar_RegisterFloat("safeArea_horizontal", 1.0f, 0.15f, 1.0f, 0x1, "Horizontal safe area as a fraction of the screen width");
	safeArea_vertical = Dvar_RegisterFloat("safeArea_vertical", 1.0f, 0.15f, 1.0f, 0x1, "Vertical safe area as a fraction of the screen height");

	Detours::X86::DetourFunction((PBYTE)0x00644CCB, (PBYTE)&ScrPlace_SetupFloatViewportDetour, Detours::X86Option::USE_CALL);
	Detours::X86::DetourFunction((PBYTE)0x00644CEE, (PBYTE)&ScrPlace_SetupFloatViewportDetour, Detours::X86Option::USE_CALL);
	Detours::X86::DetourFunction((PBYTE)0x00644D11, (PBYTE)&ScrPlace_SetupFloatViewportDetour, Detours::X86Option::USE_CALL);

}

