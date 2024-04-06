#version 330
//https://bedroomcoders.co.uk/raylib-billboards-advanced-use/

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// Custom uniforms
uniform sampler2D mask;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord) * texture(mask, fragTexCoord);
    if (texelColor.a == 0.0) discard;
    finalColor = texelColor * fragColor * colDiffuse;
}