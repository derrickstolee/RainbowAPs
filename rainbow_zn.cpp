/*
 * rainbow.cpp
 *
 *  Created on: Sep 10, 2013
 *      Author: stolee
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int unique = 1;
int singleton = 0;
int num_sols = 0;
int** aw;

int getSize(int k, int r, int n)
{
	return r * n;
}

int getIndex(int k, int r, int i, int a)
{
	return r * i + a;
}

void setColor(int k, int r, int i, int a, char* allowed, char* colors)
{
	for ( int b = 0; b < r; b++ )
	{
		if ( b == a )
		{
			allowed[getIndex(k, r, i, a)] = 1;
			colors[i] = a;
		}
		else
		{
			allowed[getIndex(k, r, i, b)] = 0;
		}
	}
}

/**
 * Do propagation. Return 1 if good, -1 if bad.
 */
int updateAllowed(char* allowed, char* colors, int k, int r, int n, int i)
{
	// s is the start of a k-AP
	int* colors_used = (int*) malloc(r * sizeof(int));
	bzero(colors_used, r * sizeof(int));

	for ( int j = 0; j < i; j++ )
	{
		colors_used[colors[j]] = 1;
	}

	int num_colors_used = 0;
	for ( int a = 0; a < r; a++ )
	{
		if ( colors_used[a] > 0 )
		{
			num_colors_used++;
		}
	}

	int open_positions = n - i;

	if ( open_positions < (r - num_colors_used) )
	{
		free(colors_used);
		return -1;
	}

	bzero(colors_used, r * sizeof(int));

	// for all differences below half
	for ( int d = 1; d < n; d++ )
	{
		// for all positions within the AP
		for ( int kp = 0; kp < k; kp++ )
		{
			// test if the k-AP with diff d and the kpth term i has:
			// 1. (k-1) colored terms
			// 2. is almost-rainbow
			bzero(colors_used, r * sizeof(int));
			int num_colored = 0;
			int is_rainbow = 1;
			int t_uncolored = 0;
			for ( int l = 0; l < k; l++ )
			{
				int t = (i + (l - kp) * d + n * n) % n;
				if ( t >= 0 && t <= i )
				{
					num_colored++;
					(colors_used[colors[t]])++;

					if ( colors_used[colors[t]] > 1 )
					{
						is_rainbow = 0;
					}
				}
				else
				{
					t_uncolored = t;
				}
			}

			if ( num_colored == k - 1 && is_rainbow == 1 )
			{
				for ( int a = 0; a < r; a++ )
				{
					if ( colors_used[a] == 0 )
					{
						// this unused color would make a rainbow AP
						allowed[getIndex(k, r, t_uncolored, a)] = 0;
					}
				}
			}
		}
	}
	free(colors_used);

	// check that all colors have an allowed position
	for ( int a = 0; a < r; a++ )
	{
		int found = 0;
		for ( int j = 0; j < n; j++ )
		{
			if ( allowed[getIndex(k, r, j, a)] == 1 )
			{
				found = 1;
				break;
			}
		}

		if ( found == 0 )
		{
			// this color does not appear!
			return -1;
		}
	}

	return 1;
}

int doSearch(char* allowed, char* colors, int size, int k, int r, int n, int cur_i)
{
	if ( cur_i >= n )
	{
		// check that all colors have an allowed position
		bool has_singleton_set = false;

		for ( int a = 0; a < r; a++ )
		{
			int found = 0;
			for ( int j = 0; j < n; j++ )
			{
				if ( colors[j] == a )
				{
					found++;

					if ( found > 1 )
					{
						break;
					}
				}
			}

			if ( found == 0 )
			{
				// this color does not appear!
				return 0;
			}

			if ( found == 1 )
			{
				has_singleton_set = true;
			}
		}

		if ( singleton == 0 || has_singleton_set )
		{
			// we are done!
			for ( int i = 0; i < n; i++ )
			{
				printf("%d ", colors[i]);
			}
			printf("\n");

			num_sols++;

			return 1;
		}

		return 0;
	}

	int max_color_so_far = -1;

	for ( int j = 0; j < cur_i; j++ )
	{
		if ( ((int) colors[j]) > max_color_so_far )
		{
			max_color_so_far = (int) colors[j];
		}
	}

	if ( max_color_so_far == r - 1 )
	{
		// no "next" color
		max_color_so_far = r - 2;
	}

	int total = 0;
	for ( int a = 0; a <= max_color_so_far + 1; a++ )
	{
		if ( allowed[getIndex(k, r, cur_i, a)] == 1 )
		{
			// push the new value
			char* new_allowed = allowed + size;
			bcopy(allowed, new_allowed, size);

			char* new_colors = colors + n;
			bcopy(colors, new_colors, n);

			setColor(k, r, cur_i, a, new_allowed, new_colors);

			int result = updateAllowed(new_allowed, new_colors, k, r, n, cur_i);

			if ( result > 0 )
			{
				// recurse!
				int result = doSearch(new_allowed, new_colors, size, k, r, n, cur_i + 1);

				if ( (unique > 0) && (result > 0) )
				{
					// only finds one...
					return result;
				}

				total += result;
			}
		}
	}

	return total;
}

int main(int argc, char** argv)
{
	// aw is a global table... useful!
	aw = (int**) malloc(100 * sizeof(int*));
	for ( int n = 0; n < 100; n++ )
	{
		aw[n] = (int*) malloc((n + 1) * sizeof(int));

		for ( int k = 0; k <= n; k++ )
		{
			aw[n][k] = -1;
		}

		aw[n][n] = n;
	}

	bool use_all_k = true;

	if ( argc > 1 && strcmp(argv[1], "k=3") == 0 )
	{
		use_all_k = false;
	}

	int n = 3;

	while ( n < 20 )
	{
		int k = 3;
		int r = 1;

		while ( k < n )
		{

			int result = 1;

			while ( result == 1 )
			{
				r++;
				int size = getSize(k, r, n);

				char* allowed = (char*) malloc((n + 2) * size * sizeof(char));
				char* colors = (char*) malloc((n + 2) * (n + 2) * sizeof(char));

				for ( int i = 0; i < (n + 1) * size; i++ )
				{
					allowed[i] = 1;
				}

				printf("\n\n\nTesting k = %2d, n = %2d, r = %2d\n", k, n, r);
				fflush(stdout);

				time_t start_time = time(NULL);
				clock_t start_clock = clock();

				singleton = 0;
				result = doSearch(allowed, colors, size, k, r, n, 0);

				clock_t end_clock = clock();
				time_t end_time = time(NULL);

				if ( end_time - start_time > 300 )
				{
					printf("Time for k = %2d, n = %2d, r = %2d : %ld seconds.\n", k, n, r, (end_time - start_time));
				}
				else
				{
					printf("Time for k = %2d, n = %2d, r = %2d : %3.2lf seconds.\n", k, n, r,
							((double) (end_clock - start_clock)) / (double) CLOCKS_PER_SEC);
				}

				if ( false && result == 1 )
				{
					// singleton version!
					time_t start_time = time(NULL);
					clock_t start_clock = clock();

					singleton = 1;
					result = doSearch(allowed, colors, size, k, r, n, 0);

					clock_t end_clock = clock();
					time_t end_time = time(NULL);

					if ( end_time - start_time > 300 )
					{
						printf("Time for k = %2d, n = %2d, r = %2d : %ld seconds.\n", k, n, r, (end_time - start_time));
					}
					else
					{
						printf("Time for k = %2d, n = %2d, r = %2d : %3.2lf seconds.\n", k, n, r,
								((double) (end_clock - start_clock)) / (double) CLOCKS_PER_SEC);
					}
				}

				free(allowed);
				free(colors);
			}
			printf("aw(Z_{n=%02d},k=%02d) = %d.\n", n, k, r);

			fflush(stdout);
			r -= 2;
			k++;
		}
		n++;
	}

	return 0;
}

