#version 330

in	vec2	  v_TexCoord;	// varying variable for passing texture coordinate from vertex shader
uniform	sampler2D u_Sampler;	// uniform variable for the texture image
uniform float     u_thr;	// threshold value
out	vec4	  fragColor;

void main() 
{
	vec3 clr = texture(u_Sampler, v_TexCoord).rgb;
	float r = step(u_thr, clr.r);
	float g = step(u_thr, clr.g);
	float b = step(u_thr, clr.b);
	fragColor = vec4(r, g, b, 1.0);
}
