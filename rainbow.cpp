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

	// if we cannot avoid a rainbow-AP for this n..
	if ( i > 0 && n - i >= k && aw[n - i][k] <= r )
	{
		// If all positions are 0, then from i to n we must have a rainbow-AP-avoiding exact r-coloring of (n-i+1).
		int all_zero = 1;
		for ( int j = 0; j <= i; j++ )
		{
			if ( colors[j] != 0 )
			{
				all_zero = 0;
			}
		}

		if ( all_zero > 0 )
		{
			return -1;
		}
	}

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

	char* colors_used = (char*) malloc(r);
	bzero(colors_used, r);
	for ( int j = 0; j <= i; j++ )
	{
		(colors_used[(int)colors[j]])++;
	}

	int num_colors_so_far = 0;
	for ( int a = 0; a < r; a++ )
	{
		if ( colors_used[a] > 0 )
		{
			num_colors_so_far++;
		}
	}

	if ( n - i < r - num_colors_so_far )
	{
		// not enough room to get the rest of the colors!
		free(colors_used);
		return -1;
	}

	// Check for almost-rainbow APs with second-to-last term i
	for ( int d = 1; d <= i / (k - 2); d++ )
	{
		if ( i + d < n )
		{
			// is this rainbow?
			bzero(colors_used, r);
			int is_rainbow = 1;
			for ( int l = 0; l < k - 1; l++ )
			{
				int t = i - (l * d);
				(colors_used[(int)colors[t]])++;

				if ( colors_used[(int)colors[t]] > 1 )
				{
					is_rainbow = 0;
				}
			}

			if ( is_rainbow )
			{
				int t = i + d;
				for ( int a = 0; a < r; a++ )
				{
					if ( colors_used[a] == 0 )
					{
						// this unused color would make a rainbow AP
						allowed[getIndex(k, r, t, a)] = 0;
					}
				}
			}
		}
	}

	free(colors_used);

	return 1;
}

int doSearch(char* allowed, char* colors, int size, int k, int r, int n, int cur_i)
{
	if ( cur_i >= n )
	{
		// check that all colors have an allowed position
		for ( int a = 0; a < r; a++ )
		{
			int found = 0;
			for ( int j = 0; j < n; j++ )
			{
				if ( colors[j] == a )
				{
					found = 1;
					break;
				}
			}

			if ( found == 0 )
			{
				// this color does not appear!
				return 0;
			}
		}

		// we are done!
		for ( int i = 0; i < n; i++ )
		{
			printf("%d ", colors[i]);
		}
		printf("\n");
		num_sols++;

		return 1;
	}

	int max_color_so_far = -1;
	for ( int j = 0; j < cur_i; j++ )
	{
		if ( colors[j] > max_color_so_far )
		{
			max_color_so_far = colors[j];
		}
	}

	if ( max_color_so_far == r - 1 )
	{
		max_color_so_far--;
	}

	int colors_found = 0;
	int total_found = 0;
	for ( int a = 0; a <= max_color_so_far + 1; a++ )
	{
		if ( allowed[getIndex(k, r, cur_i, a)] == 1 )
		{
			colors_found++;

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

				if ( unique > 0 && result > 0 )
				{
					// only finds one...
					return result;
				}

				total_found += result;
			}
		}
	}

	return total_found;
}

int main(int argc, char** argv)
{
	unique = 0;

	// aw is a global table... useful!
	aw = (int**) malloc(100 * sizeof(int*));
	for ( int n = 0; n < 100; n++ )
	{
		aw[n] = (int*) malloc((n + 5) * sizeof(int));

		for ( int k = 0; k <= n; k++ )
		{
			aw[n][k] = -1;
		}

		aw[n][n] = n;
	}

	for ( int n = 3; n < 300; n++ )
	{
		int k = 3;
		//for ( int k = 3; k <= n; k++ )
		{
			int r = k - 1;
			if ( aw[n - 1][k] > 0 )
			{
				r = aw[n - 1][k] - 3;
			}

			if ( r < 2 )
			{
				r = 2;
			}

			int result = 1;

			while ( result > 0 )
			{
				r++;
				int size = getSize(k, r, n);

				char* allowed = (char*) malloc((n + 1) * size * sizeof(char));
				char* colors = (char*) malloc((n + 1) * (n + 1) * sizeof(char));

				for ( int i = 0; i < (n + 1) * size; i++ )
				{
					allowed[i] = 1;
				}

				printf("\n\n\nTesting k = %2d, n = %2d, r = %2d\n", k, n, r);
				fflush(stdout);
				time_t start_time = time(NULL);
				clock_t start_clock = clock();

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

				free(allowed);
				free(colors);
			}

			aw[n][k] = r;
			printf("aw(n=%02d,k=%02d) = %d.\n", n, k, r);
			fflush(stdout);

		//	if ( !use_all_k || r == n )
			{
		//		break;
			}
		}
	}

	return 0;
}

