#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec3 fragPos;

// NOTE: Add here your custom variables

void main()
{
    // Send vertex attributes to fragment shader
    fragTexCoord = vertexTexCoord;
    fragPos = vertexPosition;

    // as a quick way to draw from the inside, invert vertex positions
    vec3 invertPosition = vertexPosition * vec3(-1);
    // Calculate final vertex position
    gl_Position = mvp*vec4(invertPosition, 1.0);
}