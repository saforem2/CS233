#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "bmp.h"
#include "mandelbrot.h"
#include <math.h>
// this scary-looking thing is a function pointer -
// the C equivalent of C++ function objects (functors).
// Google it if you're interested
typedef int *(*mandelbrot_fn)(float x[SIZE], float y[SIZE]);

int *
mandelbrot_scalar(float x[SIZE], float y[SIZE]) {
	static int ret[SIZE];
	float x1, y1, x2, y2;

	for (int i = 0 ; i < SIZE ; i ++) {
		x1 = y1 = 0.0;

		// Run M_ITER iterations
		for (int j = 0 ; j < M_ITER ; j ++) {
			// Calculate the real part of (x1 + y1 * i)^2 + (x + y * i)
			x2 = (x1 * x1) - (y1 * y1) + x[i];

			// Calculate the imaginary part of (x1 + y1 * i)^2 + (x + y * i)
			y2 = 2 * (x1 * y1) + y[i];

			// Use the new complex number as input for the next iteration
			x1 = x2;
			y1 = y2;
		}

		// caculate the magnitude of the result
		// We could take the square root, but instead we just
		// compare squares
		ret[i] = ((x2 * x2) + (y2 * y2)) < (M_MAG * M_MAG);
	}

	return ret;
}

int *
time_mandelbrot(float x[SIZE], float y[SIZE], mandelbrot_fn mandelbrot, const char *version) {
	struct timeval start, end;
	int *ret;

	gettimeofday(&start, NULL);
	for (int i = 0; i < ITER; i++) {
		ret = mandelbrot(x, y);
	}
	gettimeofday(&end, NULL);

	long elapsed = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
	printf("%s version: %d iterations, %ld usec\n", version, ITER, elapsed);
	return ret;
}

void
create_image(int *ret, const char *name) {
	bmp image(WIDTH, HEIGHT);
	for (int y = 0; y < HEIGHT; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			// white for points out of set, black for points in set
			uint8_t color = !ret[y * WIDTH + x] * 255;
			pixel p = { color, color, color };
			*image(x, y) = p;
		}
	}

	image.write_to_file(name);
}

int main(int argc, char **argv) {
	static float x_coords[SIZE];
	static float y_coords[SIZE];

	// y axis goes from 1 to -1
	// x axis goes from -2 to 1
	for (int y = 0; y < HEIGHT; ++y) {
		float y_coord = 1.0 - (y * 2.0 / (HEIGHT - 1));
		for (int x = 0; x < WIDTH; ++x) {
			float x_coord = -2.0 + (x * 3.0 / (WIDTH - 1));
			x_coords[y * WIDTH + x] = x_coord;
			y_coords[y * WIDTH + x] = y_coord;
		}
	}

	int *scalar_ret = time_mandelbrot(x_coords, y_coords, mandelbrot_scalar, "Scalar");
	int *vector_ret = time_mandelbrot(x_coords, y_coords, mandelbrot_vector, "Vector");
/*

	for(int i=0; i<SIZE; i++){
		if(fabs(scalar_ret[i]-vector_ret[i])>0.001)
			printf("Fail: beyond the scope");
		
}
*/
	create_image(scalar_ret, "mandelbrot-scalar.bmp");
	create_image(vector_ret, "mandelbrot-vector.bmp");

	return 0;
}
