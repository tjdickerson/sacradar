//
// Created by tjdic on 6/9/2024.
//

#include "win_opengl.h"
#include "win_support.h"

static LRESULT win_init_opengl(HDC hdc) {
    PIXELFORMATDESCRIPTOR requested_pfd = {};
    requested_pfd.nSize                 = sizeof(requested_pfd);
    requested_pfd.nVersion              = 1;
    requested_pfd.dwFlags               = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    requested_pfd.iPixelType            = PFD_TYPE_RGBA;
    requested_pfd.cColorBits            = 32;
    requested_pfd.cAlphaBits            = 8;
    requested_pfd.cDepthBits            = 24;

    int requested_pfd_id = ChoosePixelFormat(hdc, &requested_pfd);
    if (!requested_pfd_id) {
        log_message("No suitable pixel format found.");
        return -1;
    }

    PIXELFORMATDESCRIPTOR pfd = {};
    DescribePixelFormat(hdc, requested_pfd_id, sizeof(pfd), &pfd);

    if (!SetPixelFormat(hdc, requested_pfd_id, &pfd)) {
        log_message("Failed to set pixel format.");
        return -2;
    }

    HGLRC gl_rc = wglCreateContext(hdc);
    if (!gl_rc) {
        log_message("Failed to create OpenGL context.");
        return -3;
    }

    if (!wglMakeCurrent(hdc, gl_rc)) {
        log_message("Failed call to wglMakeCurrent.");
        return -4;
    }

    /*GLint*/
    const int req_attributes[] = {WGL_CONTEXT_MAJOR_VERSION_ARB,
                                  3,
                                  WGL_CONTEXT_MINOR_VERSION_ARB,
                                  2,
                                  WGL_CONTEXT_PROFILE_MASK_ARB,
                                  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                                  0};

    PFNWGLCREATECONTEXTATTRIBSARBPROC wgl_create_context_attribs_arb;
    wgl_create_context_attribs_arb = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    HGLRC new_gl_rc = wgl_create_context_attribs_arb(hdc, nullptr, req_attributes);
    if (!new_gl_rc) {
        log_message("Failed to set OpenGL Context.");
        return -5;
    }

    wglMakeCurrent(nullptr, nullptr);
    wglMakeCurrent(hdc, new_gl_rc);

    return 0;
}
