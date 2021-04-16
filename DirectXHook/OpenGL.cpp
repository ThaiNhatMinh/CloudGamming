#include "pch.h"

#include "Logger.hh"
#include "Module.hh"
#include "OpenGL.hh"
#include "RenderStream.hh"

typedef BOOL(__stdcall * twglSwapBuffers) (_In_ HDC hDc);
typedef PROC(*twglGetProcAddress_t) (LPCSTR lpszProc);
typedef void (*tglFlush)();

twglSwapBuffers pwglSwapBuffers = nullptr;
twglGetProcAddress_t pwglGetProcAddress = nullptr;
extern RenderStream* streamer;

bool hook_wglSwapBuffers(HDC hDc)
{
    return pwglSwapBuffers(hDc);
}

PROC hwglGetProcAddress(LPCSTR ProcName)
{
    LOG << ProcName << std::endl;
    return pwglGetProcAddress(ProcName);
}

bool HookOpenGL()
{
    HMODULE hMod = CheckModule("OPENGL32.DLL");
    if (hMod == NULL) return false;
    pwglSwapBuffers = GetProcAddress<twglSwapBuffers>(hMod, "wglSwapBuffers");
    pwglGetProcAddress = GetProcAddress<twglGetProcAddress_t>(hMod, "wglGetProcAddress");

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    LOG << "DetourAttach:" << DetourAttach(&(LPVOID&)pwglSwapBuffers, hook_wglSwapBuffers) << std::endl;
    LOG << "DetourAttach:" << DetourAttach(&(LPVOID&)pwglGetProcAddress, hwglGetProcAddress) << std::endl;
    DetourTransactionCommit();
    LOG << "Hook OpenGL done..." << std::endl;
    return true;
}