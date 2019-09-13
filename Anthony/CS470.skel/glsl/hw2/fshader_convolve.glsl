#version 330

in	vec2		v_TexCoord;	// varying variable for passing texture coordinate from vertex shader

uniform int		u_Wsize;	// kernel width value
uniform int		u_Hsize;	// kernel height value
uniform float		u_Kernel[128];
uniform highp float	u_StepX;	// pixel size in x dir
uniform highp float	u_StepY;	// pixel size in y dir
uniform	sampler2D	u_Sampler;	// uniform variable for the texture image

out	vec4	  fragColor;

void main() 
{
	vec2 tc = v_TexCoord;
	int  w2 = u_Wsize / 2;
	int  h2 = u_Hsize / 2;
	int  k  = 0;
	vec3 val = vec3(0.0);
	for(int i=-h2; i<=h2; ++i) {
		for(int j=-w2; j<=w2; ++j) {
			val += u_Kernel[k]*texture(u_Sampler, vec2(tc.x + j*u_StepX, tc.y + i*u_StepY)).rgb;
			++k;
		}
	}
	fragColor = vec4(val, 1.0);
}
