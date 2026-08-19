#include <math.h>

/* Newton-Raphson: r_{n+1} = (r_n + x / r_n) / 2.
 *
 * x is first scaled into [1, 4) by even powers of two, so that the
 * linear seed below is accurate to a few percent and the (quadratically
 * convergent) iteration reaches the last bit in about five steps
 * regardless of the exponent of x.
 *
 * On [1, 4) the seed lies above sqrt(x) - the chord of a concave curve -
 * and the iteration approaches the root from above, so "the estimate
 * stopped decreasing" is a safe termination condition. */
static double sqrt_positive(double x) {
	double scale;
	double r;
	double next;
	int i;

	scale = 1.;

	while (x >= 4.) {
		x *= .25;
		scale *= 2.;
	}
	while (x < 1.) {
		x *= 4.;
		scale *= .5;
	}

	r = .5 * x + .5;

	for (i = 0; i < 16; i++) {
		next = .5 * (r + x / r);
		if (next >= r) {
			break;
		}
		r = next;
	}

	return r * scale;
}

double sqrt(double x) {
	/* sqrt returns NaN if x < 0 or x is NaN */
	if ((x < 0.) || isnan(x)) {
		return NAN;
	}
	/* sqrt returns Inf if x is Inf, and +-0 if x is +-0 */
	if (isinf(x) || (x == 0.)) {
		return x;
	}

	return sqrt_positive(x);
}

float sqrtf(float x) {
	return (float)sqrt((double)x);
}

long double sqrtl(long double x) {
	return (long double)sqrt((double)x);
}
