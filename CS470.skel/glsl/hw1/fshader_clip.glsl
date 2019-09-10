#version 330

in	vec2	  v_TexCoord;	// varying variable for passing texture coordinate from vertex shader

uniform float     u_Thr1;	// threshold value
uniform float     u_Thr2;	// threshold value
uniform	sampler2D u_Sampler;	// uniform variable for the texture image

out	vec4	  fragColor;

void main() 
{
	vec3 clr = texture(u_Sampler, v_TexCoord).rgb;
	clr = clamp(clr, u_Thr1, u_Thr2);
	fragColor = vec4(clr, 1.0);
}
