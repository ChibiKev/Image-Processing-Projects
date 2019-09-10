#include <stdio.h>
#include <math.h>
#define PI 3.1415927
#define MAG(i)		(sqrt(Fr[i]*Fr[i] + Fi[i]*Fi[i]))
#define PHASE(i)	(atan2(Fi[i],Fr[i]))

main() {
	int u, x, N;
	double c, s, real, imag, cos(), sin();
	double Fr[8], Fi[8], fr[8], fi[8];

	N = 8;
	for(x=0; x<N; x++) {
		fr[x] = 2*(x+1);
		fi[x] = 0;
	}

	for(u=0; u<N; u++) {		/* compute spectrum over all freq u */
		real = imag = 0;	/* reset real, imag component of F(u) */
		for(x=0; x<N; x++) {	/* visit each input pixel */
			c =  cos(2.*PI*u*x/N);
			s = -sin(2.*PI*u*x/N);
			real += (fr[x]*c - fi[x]*s);
			imag += (fr[x]*s + fi[x]*c);
		}
		Fr[u] = real / N;
		Fi[u] = imag / N;
	}
	for(u=0; u<N; u++)
		printf("Fr[%d] = %f\t\tFi[%d] = %f\n",u,Fr[u],u,Fi[u]);
	printf("\n");
	for(u=0; u<N; u++)
		printf("|F(%d)| = %f\t\tPhase = %f\n",u,MAG(u), PHASE(u));
	printf("\n");

	for(x=0; x<N; x++) {		/* compute each output pixel */
		real = imag = 0;
		for(u=0; u<N; u++) {
			c = cos(2.*PI*u*x/N);
			s = sin(2.*PI*u*x/N);
			real += (Fr[u]*c - Fi[u]*s);
			imag += (Fr[u]*s + Fi[u]*c);
		}
		fr[x] = real;
		fi[x] = imag;
	}
	for(x=0; x<N; x++)
		printf("fr[%d] = %f\t\tfi[%d] = %f\n",x,fr[x],x,fi[x]);
}
