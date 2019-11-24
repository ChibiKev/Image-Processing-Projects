#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.1415927
#define MAG(i)		(sqrt(Fr[i]*Fr[i] + Fi[i]*Fi[i]))
#define PHASE(i)	(atan2(Fi[i],Fr[i]))

int main(int argc, char *argv[]) 
{
	if (argc == 4) {
		FILE *input, *output; 								// Create File Pointer
		char *in  = argv[1]; 								// First Argument, Input
		int dir = atoi(argv[2]); 							// Second Argument, Dir
		char *out = argv[3]; 								// Third Arugment, Output
		input  = fopen(in, "r"); 							// Read Input
		output = fopen(out, "w"); 							// Write Output

		int u, x, N;
		double c, s, real, imag;

		int width, height; 									// Declare Width and Height
		fscanf(input, "%d\t%d", &width, &height); 			// Get Values of Width and Height
		N = height; 										// N = Height
		float *Fr = malloc(sizeof(float) * N); 				// Store Values
		float *Fi = malloc(sizeof(float) * N); 				// Store Values
		float *fr = malloc(sizeof(float) * N); 				// Store Values
		float *fi = malloc(sizeof(float) * N); 				// Store Values
		fprintf(output, "%d\t%d\n", width, height); 		// Write Width and Height In First Row

		if (dir == 0) { 									// Foward DFT
			for (x = 0; x < N; x++) {						// Goes Through Input
				fscanf(input, "%f\t%f", &fr[x], &fi[x]);
			}
			for (u = 0; u < N; u++) { 						/* compute spectrum over all freq u */
				real = imag = 0; 							/* reset real, imag component of F(u) */
				for (x = 0; x < N; x++) { 					/* visit each input pixel */
					c =  cos(2.*PI*u*x/N);
					s = -sin(2.*PI*u*x/N);
					real += (fr[x]*c - fi[x]*s);
					imag += (fr[x]*s + fi[x]*c);
				}
				Fr[u] = real / N;
				Fi[u] = imag / N;
				fprintf(output, "%f\t%f\n", Fr[u], Fi[u]); 	// Write to Output
			}
		}
		else if(dir == 1) { 								// Inverse DFT
			for (x = 0; x < N; x++) {						// Goes Through Input
				fscanf(input, "%f\t%f", &Fr[x], &Fi[x]);
			}
			for (x = 0; x < N; x++) {
				real = imag = 0;
				for (u = 0; u < N; u++) {
					c = cos(2.*PI*u*x/N);
					s = sin(2.*PI*u*x/N);
					real += (Fr[u]*c - Fi[u]*s);
					imag += (Fr[u]*s + Fi[u]*c);
				}
				fr[x] = real;
				fi[x] = imag;
				fprintf(output, "%f\t%f\n", fr[x], fi[x]); 	// Write to Output
			}
		}
		else {												// Condition Check
			printf("Invalid Dir\n");
			printf("Dir = 0, Forward DFT\n");
			printf("Dir = 1, Inverse DFT\n");
		}
		free(Fr); 											// Free Fr
		free(Fi); 											// Free Fi
		free(fr); 											// Free fr
		free(fi); 											// Free fi
		fclose(input); 										// Close Input File
		fclose(output); 									// Close Output File
	}
	else {													// Condition Check
		printf("Invalid Arguments.\n");
		printf("Format: ./dtft1D Input Dir Output\n");
	}
	return 0;
}