//
// Created by tjdic on 6/9/2024.
//

#include "gl_init.h"

#include <string>

#include "external/glad/glad.h"
#include "tjd_share.h"

GLuint shapefile_shader;

std::string read_shader_from_file(const std::string &filepath);
void        init_gl_extensions();
void        load_shaders();

bool init() {
    load_shaders();
    return true;
}


void init_gl_extensions() {
    // Using glad right now and only on windows so
    // nothing to do. This would be where to set up specific
    // platform extensions.
}

GLuint create_shader(GLenum shader_type, const GLchar *source) {
    LOGINF("Creating shader.");
    GLuint shader = glCreateShader(shader_type);

    if (shader == 0) {
        LOGERR("Failed to create shader.");
        return -1;
    }
    else {
        LOGINF("Shader created, compiling.");
    }

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    const s32 buffer_size   = 512;
    GLint     return_length = 0;
    GLchar    error_buffer[buffer_size];
    glGetShaderInfoLog(shader, buffer_size, &return_length, error_buffer);

    if (return_length > 0) {
        char error_message[buffer_size * 2];
        sprintf(error_message, "Shader failed to compile: %s", error_buffer);
        LOGERR("%s", error_message);

        return -1;
    }

    LOGINF("Shader compiled: %d", shader);
    return shader;
}

GLuint gl_create_shader_program(const GLchar *vert_source, const GLchar *frag_source) {
    GLuint vert_shader = create_shader(GL_VERTEX_SHADER, vert_source);
    GLuint frag_shader = create_shader(GL_FRAGMENT_SHADER, frag_source);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vert_shader);
    glAttachShader(shader_program, frag_shader);

    glLinkProgram(shader_program);
    LOGINF("Shader created.");
    return shader_program;
}


GLuint load_shaders_from_file(std::string vert_path, std::string frag_path) {
    std::string vert_shader = read_shader_from_file(vert_path);
    std::string frag_shader = read_shader_from_file(frag_path);

    GLuint shader = gl_create_shader_program(vert_shader.c_str(), frag_shader.c_str());
    return shader;
}

GLuint load_shapefile_shaders() {
    std::string shapefile_vert_path = R"(./shaders/glsl/shapefile_vert.glsl)";
    std::string shapefile_frag_path = R"(./shaders/glsl/shapefile_frag.glsl)";

    return load_shaders_from_file(shapefile_vert_path, shapefile_frag_path);
}

void load_shaders() {
    load_shapefile_shaders();
}

std::string read_shader_from_file(const std::string &filepath) {
    FILE *fp = fopen(filepath.c_str(), "r");
    if (!fp) {
        LOGERR("Failed to find shader source: %s.\n", filepath.c_str());
        return "";
    }

    fseek(fp, 0, SEEK_END);
    const size_t file_length = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    const auto buffer = (unsigned char *)malloc(file_length + 1);
    if (const size_t bytes_read = fread(buffer, 1, file_length, fp); bytes_read == 0) {
        LOGERR("Failed to read shader.\n");
        if (buffer != nullptr)
            free(buffer);
        return "";
    }

    buffer[file_length] = '\0';
    const auto c_result = new char[file_length];
    memcpy(c_result, buffer, file_length);

    std::string result = c_result;
    return result;
}
