//
// Created by tjdic on 6/9/2024.
//

#ifndef RENDER_INFO_H
#define RENDER_INFO_H

#include "external/glad/glad.h"


struct RenderInfo {
    GLuint vao{};
    GLuint vbo{};
    GLuint shader{};

    GLint vertex_attribute{};

    GLint trans_uniform_location{};
    GLint rot_uniform_location{};
    GLint scale_uniform_location{};
    GLint color_uniform_location{};
};


#endif //RENDER_INFO_H
