//
// Created by tjdic on 6/9/2024.
//

#ifndef WIN_OPENGL_H
#define WIN_OPENGL_H


#include <windows.h>

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092

#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001

typedef HGLRC(WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hShareContext, const int *attribList);

static LRESULT win_init_opengl(HDC hdc);


#endif //WIN_OPENGL_H
