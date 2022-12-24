#include <windows.h>
#include <stdio.h>

#include <vector>

#define VK_1 0x31
#define VK_2 0x32
#define VK_3 0x33

#define ENABLED "\u001b[32mENABLED\u001b[0m"
#define DISABLED "\u001b[31mDISABLED\u001b[0m"

uintptr_t CalculateAddress(uintptr_t base, std::vector<uintptr_t> offsets)
{
    /*
        IsBadWritePtr() is used to avoid writing to data that has not been loaded yet e.g health when the user
        is on the title screen.
    */
    for (unsigned int offset : offsets) {
        if (IsBadWritePtr((LPVOID)base, 4096) != 0) {
            return 0;
        }
        base = (*(uintptr_t*)base) + offset;
    }
    if (IsBadWritePtr((LPVOID)base, 4096) != 0) {
        return 0;
    }
    return base;
}

bool SetHealth(int targetHealth, uintptr_t localPlayerAddress)
{
    int* health = (int*)CalculateAddress(localPlayerAddress, { 0xB4 });
    if (health == 0) {
        return false;
    }
    *health = targetHealth;
    return true;
}

bool SetSuperMeter(float targetSuperMeter, uintptr_t localPlayerAddress)
{
    float* superMeter = (float*)CalculateAddress(localPlayerAddress, { 0xD8 });
    if (superMeter == 0) {
        return false;
    }
    *superMeter = targetSuperMeter;
    return true;
}

bool SetCoinsForWorldA(int targetCoins, uintptr_t modBase)
{
    int* coins = (int*)CalculateAddress(modBase + 0x002675E0, { 0xA0, 0x4F0, 0x30, 0x20, 0x28, 0x10, 0x28 });
    if (coins == 0) {
        return false;
    }
    *coins = targetCoins;
    return true;
}

bool SetCoinsForWorldB(int targetCoins, uintptr_t modBase)
{
    int* coins = (int*)CalculateAddress(modBase + 0x002675E0, { 0xA0, 0x4F0, 0x30, 0x28, 0x28, 0x10, 0x28 });
    if (coins == 0) {
        return false;
    }
    *coins = targetCoins;
    return true;
}

bool SetCoinsForWorldC(int targetCoins, uintptr_t modBase)
{
    int* coins = (int*)CalculateAddress(modBase + 0x002675E0, { 0xA0, 0x4F0, 0x30, 0x30, 0x28, 0x10, 0x28 });
    if (coins == 0) {
        return false;
    }
    *coins = targetCoins;
    return true;
}

void RenderMenu(bool infiniteHealth, bool infiniteSuper, bool infiniteCoins)
{
    system("cls");

    printf("Cuphead Cheat\n\nInfinite Health: %s [1]\nInfinite Super:  %s [2]\nInfinite Coins:  %s [3]\n\n[END] to exit..\n", infiniteHealth ? ENABLED : DISABLED, infiniteSuper ? ENABLED : DISABLED, infiniteCoins ? ENABLED : DISABLED);
}

DWORD WINAPI CheatLoop(HMODULE hModule)
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    system("title Cuphead Cheat");
    system("mode con:cols=35 lines=8");

    bool infiniteHealth = false, infiniteSuper = false, infiniteCoins = false;
    
    uintptr_t modBase;

    while (!(modBase = (uintptr_t)GetModuleHandleW(L"mono.dll"))) {
        Sleep(100);
    }

    while (!GetAsyncKeyState(VK_END)) {
        RenderMenu(infiniteHealth, infiniteSuper, infiniteCoins);

        uintptr_t localPlayerAddress = CalculateAddress(modBase + 0x00264A68, { 0xA0, 0xD20, 0xE8, 0x20 });

        if (GetAsyncKeyState(VK_1)) {
            infiniteHealth = !infiniteHealth;
        }

        if (GetAsyncKeyState(VK_2)) {
            infiniteSuper = !infiniteSuper;
        }

        if (GetAsyncKeyState(VK_3)) {
            infiniteCoins = !infiniteCoins;
        }

        if (infiniteHealth) {
            SetHealth(5, localPlayerAddress);
        }

        if (infiniteSuper) {
            SetSuperMeter(50, localPlayerAddress);
        }

        if (infiniteCoins) {
            SetCoinsForWorldA(60, modBase);
            SetCoinsForWorldB(60, modBase);
            SetCoinsForWorldC(60, modBase);
        }

        Sleep(100);
    }

    fclose(f); FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);

    return 0;
}

BOOL WINAPI DllMain(
	HINSTANCE hInstDll,
	DWORD fdwReason,
	LPVOID lpReserved)
{
	switch (fdwReason)
	{
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CheatLoop, hInstDll, 0, 0));
        break;

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;

    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        break;

    case DLL_PROCESS_DETACH:
        // Perform any necessary cleanup.
        break;
	}

    return TRUE;
}
