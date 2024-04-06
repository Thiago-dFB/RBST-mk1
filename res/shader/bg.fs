#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;

// Output fragment color
out vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;

//based on https://www.shadertoy.com/view/Xltfzj
vec4 blur(sampler2D image, vec2 uv) {
  float Pi = 6.28318530718; // Pi*2
    
  // GAUSSIAN BLUR SETTINGS {{{
  float Directions = 16.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
  float Quality = 4.0; // BLUR QUALITY (Default 4.0 - More is better but slower)
  float Size = 5.0; // BLUR SIZE (Radius)
  // GAUSSIAN BLUR SETTINGS }}}
   
  vec2 Radius = Size/vec2(1280.0,720.0);
  
  // Pixel colour
  vec4 Color = texture(image, uv);
  float iterations = 1.0;
    
  // Blur calculations
  for(float d=0.0; d<Pi; d+=Pi/Directions)
  {
	  for(float i=1.0/Quality; i<=1.0; i+=1.0/Quality)
    {
		  Color += texture(image, uv + vec2(cos(d),sin(d))*Radius*i);
      iterations += 1.0;
    }
  }

  // Output to screen
  Color /= iterations;
  return Color;
}
  

//based on https://www.shadertoy.com/view/Msf3WH
vec2 hash( vec2 p ) // replace this by something better
{
	p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) );
	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise( vec2 p )
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

	vec2  i = floor( p + (p.x+p.y)*K1 );
    vec2  a = p - i + (i.x+i.y)*K2;
    float m = step(a.y,a.x); 
    vec2  o = vec2(m,1.0-m);
    vec2  b = a - o + K2;
	vec2  c = a - 1.0 + 2.0*K2;
    vec3  h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
	vec3  n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
    return dot( n, vec3(70.0) );
}

vec4 getNoise(vec2 uv)
{
  vec2 p = uv / vec2(1280.0,720.0);
  float f = 0.0;
  uv *= 256.0;
  mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );
	f  = 0.5000*noise( uv ); uv = m*uv;
	f += 0.2500*noise( uv ); uv = m*uv;
	f += 0.1250*noise( uv ); uv = m*uv;
	f += 0.0625*noise( uv ); uv = m*uv;
  f = f * smoothstep( 0.0, 0.005, abs(p.x-0.6) );	
  f = 0.8 + 0.2*f;
  return vec4(f,f,f,1.0);
}

void main()
{
    vec4 blurred = blur(texture0, fragTexCoord);
    vec4 noised = getNoise(fragTexCoord);
    fragColor = blurred * noised;
}