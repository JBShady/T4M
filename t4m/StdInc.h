#pragma once

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include <zlib.h>
#pragma comment(lib, "zlib.lib")

#include "resource.h"

#include "Hooking.h"

#include "T4.h"
#include "Utils.h"
#include "Utils\FileIO.h"
#include "io.h"
#define __thread __declspec(thread)
#define HardDebugBreak() MessageBoxA(0, __FUNCTION__, 0, 0);

#include "buildnumber.h"

#define FS_BASEGAME "data"
#define DATE __DATE__
#define TIME __TIME__
#define CONSOLEVERSION_STR "T4Me r"COMMIT_STR"> "
#define VERSION_STR "T4-SP (r"COMMIT_STR") (built " DATE " " TIME " by Nazi Zombies remastered developer JB with Clippy95)"
#define BUILDLOG_STR VERSION_STR "\nlogfile created\n"
#define SHORTVERSION_STR "2.0."COMMIT_STR
#define LONGVERSION_STR SHORTVERSION_STR " CL " DATE " " TIME
#define VERSION COMMIT
