#shader vertex
// Source code for the Vertex Shader (written in GLSL shader language):

#version 330 core

layout(location = 0) in vec4 position; // accessing the position (0th) attribute of the position array, within GLSL

void main() // the main function for the shader
{
   gl_Position = position;
}



#shader fragment
// Source code for the Fragment Shader (written in GLSL shader language):

#version 330 core

layout(location = 0) out vec4 color; // adding an additional color attribute to the vertices

void main() // the main function for the shader
{
   color = vec4(1.0, 0.5, 0.2, 1.0);
}