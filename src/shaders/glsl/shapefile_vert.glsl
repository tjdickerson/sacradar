#version 150

in vec2 vertex;

uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main() {
    gl_Position = rotation * scale * translation * vec4(vertex, 0.0, 1.0);
}
