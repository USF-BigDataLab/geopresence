




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
 *   im - The image.
 *   p  - The vertices as array of <gdPoint>s.
 *   n  - The number of vertices.
 *   c  - The color
 *
 * See also:
 *   - <gdImagePolygon>
 */
BGD_DECLARE(void) gdImageFilledPolygon (gdImagePtr im, gdPointPtr p, int n, int c)
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
	int fill_color;
	if (n <= 0) {
		return;
	}

	if (c == gdAntiAliased) {
		fill_color = im->AA_color;
	} else {
		fill_color = c;
	}
	if (!im->polyAllocated) {
		if (overflow2(sizeof (int), n)) {
			return;
		}
		im->polyInts = (int *) gdMalloc (sizeof (int) * n);
		if (!im->polyInts) {
			return;
		}
		im->polyAllocated = n;
	}
	if (im->polyAllocated < n) {
		while (im->polyAllocated < n) {
			im->polyAllocated *= 2;
		}
		if (overflow2(sizeof (int), im->polyAllocated)) {
			return;
		}
		im->polyInts = (int *) gdReallocEx (im->polyInts,
						    sizeof (int) * im->polyAllocated);
		if (!im->polyInts) {
			return;
		}
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
		gdImageLine(im, x1, miny, x2, miny, c);
		return;
	}
	pmaxy = maxy;
	/* 2.0.16: Optimization by Ilia Chipitsine -- don't waste time offscreen */
	/* 2.0.26: clipping rectangle is even better */
	if (miny < im->cy1) {
		miny = im->cy1;
	}
	if (maxy > im->cy2) {
		maxy = im->cy2;
	}
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
				im->polyInts[ints++] = (int) ((float) ((y - y1) * (x2 - x1)) /
				                              (float) (y2 - y1) + 0.5 + x1);
			} else if ((y == pmaxy) && (y == y2)) {
				im->polyInts[ints++] = x2;
			}
		}
		/*
		  2.0.26: polygons pretty much always have less than 100 points,
		  and most of the time they have considerably less. For such trivial
		  cases, insertion sort is a good choice. Also a good choice for
		  future implementations that may wish to indirect through a table.
		*/
		for (i = 1; (i < ints); i++) {
			index = im->polyInts[i];
			j = i;
			while ((j > 0) && (im->polyInts[j - 1] > index)) {
				im->polyInts[j] = im->polyInts[j - 1];
				j--;
			}
			im->polyInts[j] = index;
		}
		for (i = 0; (i < (ints-1)); i += 2) {
			/* 2.0.29: back to gdImageLine to prevent segfaults when
			  performing a pattern fill */
			gdImageLine (im, im->polyInts[i], y, im->polyInts[i + 1], y,
			             fill_color);
		}
	}
	/* If we are drawing this AA, then redraw the border with AA lines. */
	/* This doesn't work as well as I'd like, but it doesn't clash either. */
	if (c == gdAntiAliased) {
		gdImagePolygon (im, p, n, c);
	}
}


/*
	Function: gdImageLine

	Bresenham as presented in Foley & Van Dam.
*/
BGD_DECLARE(void) gdImageLine (gdImagePtr im, int x1, int y1, int x2, int y2, int color)
{
	int dx, dy, incr1, incr2, d, x, y, xend, yend, xdirflag, ydirflag;
	int wid;
	int w, wstart;
	int thick;

	if (color == gdAntiAliased) {
		/*
		  gdAntiAliased passed as color: use the much faster, much cheaper
		  and equally attractive gdImageAALine implementation. That
		  clips too, so don't clip twice.
		*/
		gdImageAALine(im, x1, y1, x2, y2, im->AA_color);
		return;
	}
	/* 2.0.10: Nick Atty: clip to edges of drawing rectangle, return if no
	   points need to be drawn. 2.0.26, TBB: clip to edges of clipping
	   rectangle. We were getting away with this because gdImageSetPixel
	   is used for actual drawing, but this is still more efficient and opens
	   the way to skip per-pixel bounds checking in the future. */

	if (clip_1d (&x1, &y1, &x2, &y2, im->cx1, im->cx2) == 0)
		return;
	if (clip_1d (&y1, &x1, &y2, &x2, im->cy1, im->cy2) == 0)
		return;
	thick = im->thick;

	dx = abs (x2 - x1);
	dy = abs (y2 - y1);

	if (dx == 0) {
		gdImageVLine(im, x1, y1, y2, color);
		return;
	} else if (dy == 0) {
		gdImageHLine(im, y1, x1, x2, color);
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
			gdImageSetPixel (im, x, w, color);

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
					gdImageSetPixel (im, x, w, color);
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
					gdImageSetPixel (im, x, w, color);
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

		/* Set up line thickness */
		wstart = x - wid / 2;
		for (w = wstart; w < wstart + wid; w++)
			gdImageSetPixel (im, w, y, color);

		if (((x2 - x1) * xdirflag) > 0) {
			while (y < yend) {
				y++;
				if (d < 0) {
					d += incr1;
				} else {
					x++;
					d += incr2;
				}
				wstart = x - wid / 2;
				for (w = wstart; w < wstart + wid; w++)
					gdImageSetPixel (im, w, y, color);
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
				wstart = x - wid / 2;
				for (w = wstart; w < wstart + wid; w++)
					gdImageSetPixel (im, w, y, color);
			}
		}
	}

}

static void gdImageHLine(gdImagePtr im, int y, int x1, int x2, int col)
{
	if (im->thick > 1) {
		int thickhalf = im->thick >> 1;
		_gdImageFilledHRectangle(im, x1, y - thickhalf, x2, y + im->thick - thickhalf - 1, col);
	} else {
		if (x2 < x1) {
			int t = x2;
			x2 = x1;
			x1 = t;
		}

		for (; x1 <= x2; x1++) {
			gdImageSetPixel(im, x1, y, col);
		}
	}
	return;
}

static void gdImageVLine(gdImagePtr im, int x, int y1, int y2, int col)
{
	if (im->thick > 1) {
		int thickhalf = im->thick >> 1;
		gdImageFilledRectangle(im, x - thickhalf, y1, x + im->thick - thickhalf - 1, y2, col);
	} else {
		if (y2 < y1) {
			int t = y1;
			y1 = y2;
			y2 = t;
		}

		for (; y1 <= y2; y1++) {
			gdImageSetPixel(im, x, y1, col);
		}
	}
	return;
}

