# include <stdlib.h>
# include <stdio.h>

# include <omp.h>

int main ( int argc, char *argv[] );
int prime_default ( int n );
int prime_static ( int n );
int prime_dynamic ( int n );

int main ( int argc, char *argv[] ) {
	int n;
	int n_factor;
	int n_hi;
	int n_lo;
	int primes;
	double time1;
	double time2;
	double time3;

	printf ( "\n  Number of processors available = %d\n", omp_get_num_procs ( )  );
	printf ( "  Number of threads =              %d\n", omp_get_max_threads ( )  );

	n_lo = 1;
	n_hi = 131072;
	n_factor = 2;

	printf ( "\n" );
	printf ( "                           Default        Static       Dynamic\n" );
	printf ( "         N     Pi(N)          Time          Time          Time\n" );
	printf ( "\n" );

	n = n_lo;

	while ( n <= n_hi )
	{
		time1 = omp_get_wtime ( );
		primes = prime_default ( n );
		time1 = omp_get_wtime ( ) - time1;

		time2 = omp_get_wtime ( );
		primes = prime_static ( n );
		time2 = omp_get_wtime ( ) - time2;

		time3 = omp_get_wtime ( );
		primes = prime_dynamic ( n );
		time3 = omp_get_wtime ( ) - time3;

		printf ( "  %8d  %8d  %12f  %12f  %12f\n", n, primes, time1, time2, time3 );

		n = n * n_factor;
	}

	printf("\n");

	return 0;
}

int prime_default ( int n ) {
	int i;
	int j;
	int prime;
	int total = 0;

	for ( i = 2; i <= n; i++ )
	{
		prime = 1;

		for ( j = 2; j < i; j++ ){
			if ( i % j == 0 ){
				prime = 0;
				break;
			}
		}
		total = total + prime;
	}
	return total;
}

int prime_static ( int n ) {
	int i;
	int j;
	int prime;
	int total = 0;

	for ( i = 2; i <= n; i++ ){
		prime = 1;
		for ( j = 2; j < i; j++ ){
			if ( i % j == 0 ) {
				prime = 0;
				break;
			}
		}
		total = total + prime;
	}
	return total;
}

int prime_dynamic ( int n ) {
	int i;
	int j;
	int prime;
	int total = 0;

	for ( i = 2; i <= n; i++ ) {
		prime = 1;
		for ( j = 2; j < i; j++ ) {
			if ( i % j == 0 ) {
				prime = 0;
				break;
			}
		}
		total = total + prime;
	}
	return total;
}
