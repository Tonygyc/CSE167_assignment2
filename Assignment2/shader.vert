

/**

#version 330 core
// NOTE: Do NOT use any version older than 330! Bad things will happen!

// This is an example vertex shader. GLSL is very similar to C.
// You can define extra functions if needed, and the main() function is
// called when the vertex shader gets run.
// The vertex shader gets called once per vertex.

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 modelview;

// Outputs of the vertex shader are the inputs of the same name of the fragment shader.
// The default output, gl_Position, should be assigned something. You can define as many
// extra outputs as you need.
out float sampleExtraOutput;
out vec3 normalVector;

void main()
{
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0);
    sampleExtraOutput = 1.0f;
    
    //output the normal vectors to the fragment shader
    normalVector = normals;
}



**/

// BELOW TAKEN AND MODIFIED DIRECTLY FROM PROVIDED LINK ON ASSIGNMENT WRITEUP

#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normals;
//layout (location = 2) in vec2 texCoords;

out vec3 Normal;
out vec3 FragPos;
//out vec2 TexCoords;

uniform mat4 model;
//uniform mat4 view;
uniform mat4 projection;
uniform mat4 modelview;
uniform int shaderMode;

void main()
{
    gl_Position = projection * modelview * vec4(position.x, position.y, position.z, 1.0f);
    FragPos = vec3(model * vec4(position.x, position.y, position.z, 1.0f));
    //Normal = mat3(transpose(inverse(model))) * normal;
    
    Normal = normals;
    
    //TexCoords = texCoords;
}

