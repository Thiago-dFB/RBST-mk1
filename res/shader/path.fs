#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// Custom uniforms
uniform float scroll;

void main()
{
    vec4 texelColor = texture(texture0, vec2(fragTexCoord.x*2000 + scroll*10, fragTexCoord.y));
    finalColor = texelColor * fragColor * colDiffuse;
}