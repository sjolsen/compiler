/*
  RankSort v1.0: This code uses the rank-sort algorithm to sort an array of
  integers.

  Copyright (c) 2013, Texas State University-San Marcos. All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted for academic, research, experimental, or personal use provided
  that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
  * Neither the name of Texas State University-San Marcos nor the names of its
  contributors may be used to endorse or promote products derived from this
  software without specific prior written permission.

  For all other uses, please contact the Office for Commercialization and Industry
  Relations at Texas State University-San Marcos <http://www.txstate.edu/ocir/>.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  Author: Martin Burtscher <burtscher@txstate.edu>
*/

void exit (int retval)
{
}

int atoi (char str [])
{
}

int main (int argc,
          char argv [])
{
	int i;
	int j;
	int cnt;
	int val;
	int size;
	int nthreads;

	int a;
	int b;

	int start;
	int end;

	/* check command line */
	if (argc != 3)
	{
		exit (0-1);
	}

	size = atoi (argv [1]);
	if (size < 1)
	{
		exit (0-1);
	}

	nthreads = atoi (argv [2]);
	if (nthreads < 1)
	{
		exit (0-1);
	}

	/* allocate arrays */
	a = malloc (size * 4);
	b = malloc (size * 4);

	if (or ((a == NULL), (b == NULL)))
	{
		exit (0-1);
	}

	/* generate input */
	i = 0;
	while (i < size)
	{
		a [i] = i;
		b [i] = 0;
		i = i + 1;
	}
	printf ("sorting %d values with %d threads\n", size, nthreads);

	/* start time */
	gettimeofday (addressof (start), NULL);

	/* sort the values */

	i = 0;
	while (i < size)
	{
		cnt = 0;
		val = a [i];
		j = 0;
		while (j < size)
		{
			if (val > a [j])
				cnt = cnt + 1;
			j = j + 1;
		}
		b [cnt] = val;
		i = i + 1;
	}

	/* end time */
	gettimeofday (addressof (end), NULL);
	printf ("runtime: %.4lf s\n", tvsec (end) + tvusec (end) / 1000000 - tvsec (start) - tvusec (start) / 1000000);

	/* verify result */
	i = 1;
	while (and ((i < size), (b [i - 1] < b [i])))
		i = i + 1;
	if (i < size)
	{
		printf ("NOT sorted\n\n");
		i = 0;
		while (i < size)
		{
			fprintf (stderr, "%d ", b [i]);
			fprintf (stderr, "\n");
			i = i + 1;
		}
	}
	else {}

	free (a);
	free (b);
	return 0;
}
