#version 330

in	vec2	  	v_TexCoord;	// varying variable for passing texture coordinate from vertex shader

uniform int		u_Size;	// blur width value
uniform highp float	u_StepX;
uniform highp float	u_StepY;
uniform	sampler2D	u_Sampler;	// uniform variable for the texture image

out	vec4	  	fragColor;

void main() {
	
	const int max_size = 25;

	float r[max_size];
	float g[max_size];
	float b[max_size];

	vec2 tc  = v_TexCoord;
	int  w2  = u_Size / 2;
	int  k   = 0;
	for(int i=-w2; i<=w2; ++i) {
		for(int j=-w2; j<=w2; ++j) {
			vec3 clr = texture(u_Sampler, vec2(tc.x + j*u_StepX, tc.y + i*u_StepY)).rgb;
			r[k] = clr.r;
			g[k] = clr.g;
			b[k] = clr.b;
			++k;
		}
	}
	int total = u_Size*u_Size;
	for(int i = 0; i < total; ++i) {
		// Find the minimum element in unsorted array
		int min_idx = i;
		for (int j = i+1; j < total; ++j)
			if (r[j] < r[min_idx])
				min_idx = j;
 
		// Swap the found minimum element with the first element
		float temp = r[min_idx];
		r[min_idx] = r[i];
		r[i] = temp;
	}
	for(int i = 0; i < total; ++i) {
		// Find the minimum element in unsorted array
		int min_idx = i;
		for (int j = i+1; j < total; ++j)
			if (g[j] < g[min_idx])
				min_idx = j;
 
		// Swap the found minimum element with the first element
		float temp = g[min_idx];
		g[min_idx] = g[i];
		g[i] = temp;
	}
	for(int i = 0; i < total; ++i) {
		// Find the minimum element in unsorted array
		int min_idx = i;
		for (int j = i+1; j < total; ++j)
			if (b[j] < b[min_idx])
				min_idx = j;
 
		// Swap the found minimum element with the first element
		float temp = b[min_idx];
		b[min_idx] = b[i];
		b[i] = temp;
	}
	int mid = total / 2;
	fragColor = vec4(r[mid], g[mid], b[mid], 1.0);
}
