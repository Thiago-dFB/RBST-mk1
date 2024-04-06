#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec3 fragPos;

// Output fragment color
out vec4 fragColor;

// Custom uniforms
uniform float filter;
uniform vec2 pos;
uniform float fade;

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

float getNoise(vec2 uv)
{
  vec2 p = uv / vec2(100.0);
  float f = 0.0;
  uv *= 4.0;
  mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );
	f  = 0.5000*noise( uv ); uv = m*uv;
	f += 0.2500*noise( uv ); uv = m*uv;
	f += 0.1250*noise( uv ); uv = m*uv;
	f += 0.0625*noise( uv ); uv = m*uv;
  f = f * smoothstep( 0.0, 0.005, abs(p.x-0.6) );
  return f;
}

void main()
{
    vec2 pseudoCoord = vec2(fragPos.x + fragPos.z + pos.x + pos.y, fragPos.y);
    float noise = 0.2 * getNoise(pseudoCoord);
    //filter [0.0, 1.0] the lower the more hidden top and bottom are
    noise = noise + 20.0 * max(0.0, abs(fragPos.y) - filter);
    //fade = 0.0 -> [0.0, 0.2]
    //fade = 0.5 -> [0.5, 1.0]
    //fade = 1.0 -> [1.0, 1.8]
    noise = fade + noise * (1 + fade*3);
    //once above threshold, discard
    if (noise > 0.5) discard;
    //color lerps from 0x8000FF to 0xFFFFFF, alpha from 0.2 to 1.0
    fragColor = vec4(0.5+ noise*0.5, noise, 1.0, noise*2);
}