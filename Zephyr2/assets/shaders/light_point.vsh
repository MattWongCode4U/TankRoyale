#version 330 core

layout (location = 0) in vec3 aPos;     // Vertex position.
out vec2 uv;                            // Texture coordinate.

//
// Vertex shader for point lights.
//
void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);    // Pass vertex position as gl_Position.
    uv = (aPos.xy + vec2(1, 1)) / 2.0;  // Calculate texture coordinate.
}