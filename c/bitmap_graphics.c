#include "roaring.h"
#include "geode.h"
#include "bitmap_graphics.h"
#include <math.h>

/**
 * Function: gdImageFilledPolygon
 *
 * Draws a filled polygon
 *
 * The polygon is filled using the even-odd fillrule what can leave unfilled
 * regions inside of self-intersecting polygons. This behavior might change in
 * a future version.
 *
 * Parameters:
 *   im - The roaring bitmap.
 *   p  - The vertices as array of <geodePoint>s.
 *   n  - The number of vertices.
 *
 * See also:
 *   - <gdImagePolygon>
 */
void bmp_filled_polygon (roaring_bitmap_t *bmp, geodePointPtr p, int n, int width, int height)
{
	int i;
	int j;
	int index;
	int y;
	int miny, maxy, pmaxy;
	int x1, y1;
	int x2, y2;
	int ind1, ind2;
	int ints;
    int *poly_ints;
	if (n <= 0 || n > 100) {
		return;
	}

    poly_ints = (int *) malloc(sizeof (int) * n);
    if (!poly_ints) {
        return;
    }

	miny = p[0].y;
	maxy = p[0].y;
	for (i = 1; (i < n); i++) {
		if (p[i].y < miny) {
			miny = p[i].y;
		}
		if (p[i].y > maxy) {
			maxy = p[i].y;
		}
	}
	/* necessary special case: horizontal line */
	if (n > 1 && miny == maxy) {
		x1 = x2 = p[0].x;
		for (i = 1; (i < n); i++) {
			if (p[i].x < x1) {
				x1 = p[i].x;
			} else if (p[i].x > x2) {
				x2 = p[i].x;
			}
		}
		bmp_draw_line(bmp, x1, miny, x2, miny, width, height);
		return;
	}
	pmaxy = maxy;

	/* Fix in 1.3: count a vertex only once */
	for (y = miny; (y <= maxy); y++) {
		ints = 0;
		for (i = 0; (i < n); i++) {
			if (!i) {
				ind1 = n - 1;
				ind2 = 0;
			} else {
				ind1 = i - 1;
				ind2 = i;
			}
			y1 = p[ind1].y;
			y2 = p[ind2].y;
			if (y1 < y2) {
				x1 = p[ind1].x;
				x2 = p[ind2].x;
			} else if (y1 > y2) {
				y2 = p[ind1].y;
				y1 = p[ind2].y;
				x2 = p[ind1].x;
				x1 = p[ind2].x;
			} else {
				continue;
			}

			/* Do the following math as float intermediately, and round to ensure
			 * that Polygon and FilledPolygon for the same set of points have the
			 * same footprint. */


			if ((y >= y1) && (y < y2)) {
				poly_ints[ints++] = (int) ((float) ((y - y1) * (x2 - x1)) /
				                              (float) (y2 - y1) + 0.5 + x1);
			} else if ((y == pmaxy) && (y == y2)) {
				poly_ints[ints++] = x2;
			}
		}
		/*
		  2.0.26: polygons pretty much always have less than 100 points,
		  and most of the time they have considerably less. For such trivial
		  cases, insertion sort is a good choice. Also a good choice for
		  future implementations that may wish to indirect through a table.
		*/
		for (i = 1; (i < ints); i++) {
			index = poly_ints[i];
			j = i;
			while ((j > 0) && (poly_ints[j - 1] > index)) {
				poly_ints[j] = poly_ints[j - 1];
				j--;
			}
			poly_ints[j] = index;
		}
		for (i = 0; (i < (ints-1)); i += 2) {
			/* 2.0.29: back to gdImageLine to prevent segfaults when
			  performing a pattern fill */
			bmp_draw_line (bmp, poly_ints[i], y, poly_ints[i + 1], y, width, height);
		}
	}
}


/*
	Function: bmp_draw_line
	Bresenham as presented in Foley & Van Dam.
*/
void bmp_draw_line (roaring_bitmap_t *bmp, int x1, int y1, int x2, int y2, int width, int height)
{
	int dx, dy, incr1, incr2, d, x, y, xend, yend, xdirflag, ydirflag;
	int wid;
	int w, wstart;
	int thick = 1;

	/* 2.0.10: Nick Atty: clip to edges of drawing rectangle, return if no
	   points need to be drawn. 2.0.26, TBB: clip to edges of clipping
	   rectangle. We were getting away with this because gdImageSetPixel
	   is used for actual drawing, but this is still more efficient and opens
	   the way to skip per-pixel bounds checking in the future. */

	if (clip_1d (&x1, &y1, &x2, &y2, 0, width) == 0)
		return;
	if (clip_1d (&y1, &x1, &y2, &x2, 0, height) == 0)
		return;

	dx = abs (x2 - x1);
	dy = abs (y2 - y1);

	if (dx == 0) {
		if (y2 < y1) {
			int t = y1;
			y1 = y2;
			y2 = t;
		}

		for (; y1 <= y2; y1++) {
			roaring_bitmap_add(bmp, y1 * width + x1);
		}
		return;
	} else if (dy == 0) {
        if (x2 < x1) {
			int t = x2;
			x2 = x1;
			x1 = t;
		}
        double start = width * y1;
        roaring_bitmap_add_range(bmp, start + x1, start + x2);
		return;
	}

	if (dy <= dx) {
		/* More-or-less horizontal. use wid for vertical stroke */
		/* Doug Claar: watch out for NaN in atan2 (2.0.5) */

		/* 2.0.12: Michael Schwartz: divide rather than multiply;
			  TBB: but watch out for /0! */
		double ac = cos (atan2 (dy, dx));
		if (ac != 0) {
			wid = thick / ac;
		} else {
			wid = 1;
		}
		if (wid == 0) {
			wid = 1;
		}
		d = 2 * dy - dx;
		incr1 = 2 * dy;
		incr2 = 2 * (dy - dx);
		if (x1 > x2) {
			x = x2;
			y = y2;
			ydirflag = (-1);
			xend = x1;
		} else {
			x = x1;
			y = y1;
			ydirflag = 1;
			xend = x2;
		}

		/* Set up line thickness */
		wstart = y - wid / 2;
		for (w = wstart; w < wstart + wid; w++)
			roaring_bitmap_add(bmp, x + w * width);

		if (((y2 - y1) * ydirflag) > 0) {
			while (x < xend) {
				x++;
				if (d < 0) {
					d += incr1;
				} else {
					y++;
					d += incr2;
				}
				wstart = y - wid / 2;
				for (w = wstart; w < wstart + wid; w++)
			        roaring_bitmap_add(bmp, x + w * width);
			}
		} else {
			while (x < xend) {
				x++;
				if (d < 0) {
					d += incr1;
				} else {
					y--;
					d += incr2;
				}
				wstart = y - wid / 2;
				for (w = wstart; w < wstart + wid; w++)
			        roaring_bitmap_add(bmp, x + w * width);
			}
		}
	} else {
		/* More-or-less vertical. use wid for horizontal stroke */
		/* 2.0.12: Michael Schwartz: divide rather than multiply;
		   TBB: but watch out for /0! */
		double as = sin (atan2 (dy, dx));
		if (as != 0) {
			wid = thick / as;
		} else {
			wid = 1;
		}
		if (wid == 0)
			wid = 1;

		d = 2 * dx - dy;
		incr1 = 2 * dx;
		incr2 = 2 * (dx - dy);
		if (y1 > y2) {
			y = y2;
			x = x2;
			yend = y1;
			xdirflag = (-1);
		} else {
			y = y1;
			x = x1;
			yend = y2;
			xdirflag = 1;
		}

		wstart = (x - wid / 2) + (y * width);
		roaring_bitmap_add_range(bmp, wstart, wstart + wid);

		if (((x2 - x1) * xdirflag) > 0) {
			while (y < yend) {
				y++;
				if (d < 0) {
					d += incr1;
				} else {
					x++;
					d += incr2;
				}
                wstart = (x - wid / 2) + (y * width);
                roaring_bitmap_add_range(bmp, wstart, wstart + wid);
			}
		} else {
			while (y < yend) {
				y++;
				if (d < 0) {
					d += incr1;
				} else {
					x--;
					d += incr2;
				}
                wstart = (x - wid / 2) + (y * width);
                roaring_bitmap_add_range(bmp, wstart, wstart + wid);
			}
		}
	}
}


/* 2.0.10: before the drawing routines, some code to clip points that are
 * outside the drawing window.  Nick Atty (nick@canalplan.org.uk)
 *
 * This is the Sutherland Hodgman Algorithm, as implemented by
 * Duvanenko, Robbins and Gyurcsik - SH(DRG) for short.  See Dr Dobb's
 * Journal, January 1996, pp107-110 and 116-117
 *
 * Given the end points of a line, and a bounding rectangle (which we
 * know to be from (0,0) to (SX,SY)), adjust the endpoints to be on
 * the edges of the rectangle if the line should be drawn at all,
 * otherwise return a failure code */

/* this does "one-dimensional" clipping: note that the second time it
   is called, all the x parameters refer to height and the y to width
   - the comments ignore this (if you can understand it when it's
   looking at the X parameters, it should become clear what happens on
   the second call!)  The code is simplified from that in the article,
   as we know that gd images always start at (0,0) */

/* 2.0.26, TBB: we now have to respect a clipping rectangle, it won't
	necessarily start at 0. */

int clip_1d (int *x0, int *y0, int *x1, int *y1, int mindim, int maxdim)
{
	double m;			/* gradient of line */
	if (*x0 < mindim) {
		/* start of line is left of window */
		if (*x1 < mindim)		/* as is the end, so the line never cuts the window */
			return 0;
		m = (*y1 - *y0) / (double) (*x1 - *x0);	/* calculate the slope of the line */
		/* adjust x0 to be on the left boundary (ie to be zero), and y0 to match */
		*y0 -= (int)(m * (*x0 - mindim));
		*x0 = mindim;
		/* now, perhaps, adjust the far end of the line as well */
		if (*x1 > maxdim) {
			*y1 += m * (maxdim - *x1);
			*x1 = maxdim;
		}
		return 1;
	}
	if (*x0 > maxdim) {
		/* start of line is right of window -
		complement of above */
		if (*x1 > maxdim)		/* as is the end, so the line misses the window */
			return 0;
		m = (*y1 - *y0) / (double) (*x1 - *x0);	/* calculate the slope of the line */
		*y0 += (int)(m * (maxdim - *x0));	/* adjust so point is on the right
							   boundary */
		*x0 = maxdim;
		/* now, perhaps, adjust the end of the line */
		if (*x1 < mindim) {
			*y1 -= (int)(m * (*x1 - mindim));
			*x1 = mindim;
		}
		return 1;
	}
	/* the final case - the start of the line is inside the window */
	if (*x1 > maxdim) {
		/* other end is outside to the right */
		m = (*y1 - *y0) / (double) (*x1 - *x0);	/* calculate the slope of the line */
		*y1 += (int)(m * (maxdim - *x1));
		*x1 = maxdim;
		return 1;
	}
	if (*x1 < mindim) {
		/* other end is outside to the left */
		m = (*y1 - *y0) / (double) (*x1 - *x0);	/* calculate the slope of the line */
		*y1 -= (int)(m * (*x1 - mindim));
		*x1 = mindim;
		return 1;
	}
	/* only get here if both points are inside the window */
	return 1;
}

/* end of line clipping code */
