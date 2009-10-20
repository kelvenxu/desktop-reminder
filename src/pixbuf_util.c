/*
 * GQview
 * (C) 2004 John Ellis
 *
 * Author: John Ellis
 *
 * This software is released under the GNU General Public License (GNU GPL).
 * Please read the included file COPYING for more information.
 * This software comes with no warranty of any kind, use at your own risk!
 */


#include "pixbuf_util.h"
#include <string.h>

GdkPixbuf *
pixbuf_render_text(GtkWidget *da, const char *str, int point_size, int r, int g, int b, 
		int *w, int *h)
{
	int width;
	int height;
	PangoLayout *layout = gtk_widget_create_pango_layout(da, NULL);
	PangoFontDescription *desc = pango_font_description_new();
	pango_font_description_set_size(desc, point_size * PANGO_SCALE);
	pango_layout_set_font_description(layout, desc);
	pango_font_description_free(desc);
	pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);
	pango_layout_set_text(layout, str, -1);
	pango_layout_get_pixel_size(layout, &width, &height);

	GdkPixbuf *pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, height);
	pixbuf_set_rect_fill(pixbuf, 0, 0, width, height, 0, 0, 0, 0);
	pixbuf_draw_layout(pixbuf, layout, da, 0, 0, r, g, b, 255);
	//gdk_draw_layout(da->window, da->style->fg_gc[GTK_STATE_NORMAL], 0, 0, layout);
	g_object_unref(G_OBJECT(layout));

	*w = width;
	*h = height;
	return pixbuf;
}

/*
 * Fills region of pixbuf at x,y over w,h
 * with colors red (r), green (g), blue (b)
 * applying alpha (a), use a=255 for solid.
 */
void pixbuf_draw_rect_fill(GdkPixbuf *pb,
			   gint x, gint y, gint w, gint h,
			   gint r, gint g, gint b, gint a)
{
	gint p_alpha;
	gint pw, ph, prs;
	guchar *p_pix;
	guchar *pp;
	gint i, j;

	if (!pb) return;

	pw = gdk_pixbuf_get_width(pb);
	ph = gdk_pixbuf_get_height(pb);

	if (x < 0 || x + w > pw) return;
	if (y < 0 || y + h > ph) return;

	p_alpha = gdk_pixbuf_get_has_alpha(pb);
	prs = gdk_pixbuf_get_rowstride(pb);
	p_pix = gdk_pixbuf_get_pixels(pb);

        for (i = 0; i < h; i++)
		{
		pp = p_pix + (y + i) * prs + (x * (p_alpha ? 4 : 3));
		for (j = 0; j < w; j++)
			{
			*pp = (r * a + *pp * (256-a)) >> 8;
			pp++;
			*pp = (g * a + *pp * (256-a)) >> 8;
			pp++;
			*pp = (b * a + *pp * (256-a)) >> 8;
			pp++;
			if (p_alpha) pp++;
			}
		}
}


void pixbuf_set_rect_fill(GdkPixbuf *pb,
			  gint x, gint y, gint w, gint h,
			  gint r, gint g, gint b, gint a)
{
	gint p_alpha;
	gint pw, ph, prs;
	guchar *p_pix;
	guchar *pp;
	gint i, j;

	if (!pb) return;

	pw = gdk_pixbuf_get_width(pb);
	ph = gdk_pixbuf_get_height(pb);

	if (x < 0 || x + w > pw) return;
	if (y < 0 || y + h > ph) return;

	p_alpha = gdk_pixbuf_get_has_alpha(pb);
	prs = gdk_pixbuf_get_rowstride(pb);
	p_pix = gdk_pixbuf_get_pixels(pb);

        for (i = 0; i < h; i++)
		{
		pp = p_pix + (y + i) * prs + (x * (p_alpha ? 4 : 3));
		for (j = 0; j < w; j++)
			{
			*pp = r; pp++;
			*pp = g; pp++;
			*pp = b; pp++;
			if (p_alpha) { *pp = a; pp++; }
			}
		}
}




/*
 *-----------------------------------------------------------------------------
 * pixbuf text rendering
 *-----------------------------------------------------------------------------
 */

static void pixbuf_copy_font(GdkPixbuf *src, gint sx, gint sy,
			     GdkPixbuf *dest, gint dx, gint dy,
			     gint w, gint h,
			     guint8 r, guint8 g, guint8 b, guint8 a)
{
	gint sw, sh, srs;
	gint s_alpha;
	gint s_step;
	guchar *s_pix;
	gint dw, dh, drs;
	gint d_alpha;
	gint d_step;
	guchar *d_pix;

	guchar *sp;
	guchar *dp;
	gint i, j;

	if (!src || !dest) return;

	sw = gdk_pixbuf_get_width(src);
	sh = gdk_pixbuf_get_height(src);

	if (sx < 0 || sx + w > sw) return;
	if (sy < 0 || sy + h > sh) return;

	dw = gdk_pixbuf_get_width(dest);
	dh = gdk_pixbuf_get_height(dest);

	if (dx < 0 || dx + w > dw) return;
	if (dy < 0 || dy + h > dh) return;

	s_alpha = gdk_pixbuf_get_has_alpha(src);
	d_alpha = gdk_pixbuf_get_has_alpha(dest);
	srs = gdk_pixbuf_get_rowstride(src);
	drs = gdk_pixbuf_get_rowstride(dest);
	s_pix = gdk_pixbuf_get_pixels(src);
	d_pix = gdk_pixbuf_get_pixels(dest);

	s_step = (s_alpha) ? 4 : 3;
	d_step = (d_alpha) ? 4 : 3;

	for (i = 0; i < h; i++)
		{
		sp = s_pix + (sy + i) * srs + sx * s_step;
		dp = d_pix + (dy + i) * drs + dx * d_step;
		for (j = 0; j < w; j++)
			{
			if (*sp)
				{
				guint8 asub;

				asub = a * sp[0] / 255;
				*dp = (r * asub + *dp * (256-asub)) >> 8;
				dp++;
				asub = a * sp[1] / 255;
				*dp = (g * asub + *dp * (256-asub)) >> 8;
				dp++;
				asub = a * sp[2] / 255;
				*dp = (b * asub + *dp * (256-asub)) >> 8;
				dp++;

				if (d_alpha)
					{
					*dp = MAX(*dp, a * ((sp[0] + sp[1] + sp[2]) / 3) / 255);
					dp++;
					}
				}
			else
				{
				dp += d_step;
				}

			sp += s_step;
			}
		}
}

void pixbuf_draw_layout(GdkPixbuf *pixbuf, PangoLayout *layout, GtkWidget *widget,
			gint x, gint y,
			guint8 r, guint8 g, guint8 b, guint8 a)
{
	GdkPixmap *pixmap;
	GdkPixbuf *buffer;
	gint w, h;
	GdkGC *gc;
	gint sx, sy;
	gint dw, dh;

	if (!widget || !widget->window) return;

	pango_layout_get_pixel_size(layout, &w, &h);
	pixmap = gdk_pixmap_new(widget->window, w, h, -1);

	gc = gdk_gc_new(widget->window);
	gdk_gc_copy(gc, widget->style->black_gc);
	gdk_draw_rectangle(pixmap, gc, TRUE, 0, 0, w, h);
	gdk_gc_copy(gc, widget->style->white_gc);
	gdk_draw_layout(pixmap, gc, 0, 0, layout);
	g_object_unref(gc);

	buffer = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, w, h);
	gdk_pixbuf_get_from_drawable(buffer, pixmap,
				     gdk_drawable_get_colormap(widget->window),
				     0, 0, 0, 0, w, h);
	g_object_unref(pixmap);

	sx = 0;
	sy = 0;
	dw = gdk_pixbuf_get_width(pixbuf);
	dh = gdk_pixbuf_get_height(pixbuf);

	if (x < 0)
		{
		w += x;
		sx = -x;
		x = 0;
		}

	if (y < 0)
		{
		h += y;
		sy = -y;
		y = 0;
		}

	if (x + w > dw)	w = dw - x;
	if (y + h > dh) h = dh - y;

	pixbuf_copy_font(buffer, 0, 0,
			 pixbuf, x, y, w, h,
			 r, g, b, a);

	g_object_unref(buffer);
}

