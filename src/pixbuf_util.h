
#ifndef __PIXBUF_UTIL_H__
#define __PIXBUF_UTIL_H__  1

#include <gtk/gtk.h>
void pixbuf_draw_rect_fill(GdkPixbuf *pb,
			   gint x, gint y, gint w, gint h,
			   gint r, gint g, gint b, gint a);
void pixbuf_draw_layout(GdkPixbuf *pixbuf, PangoLayout *layout, GtkWidget *widget,
			gint x, gint y,
			guint8 r, guint8 g, guint8 b, guint8 a);
void pixbuf_set_rect_fill(GdkPixbuf *pb,
			  gint x, gint y, gint w, gint h,
			  gint r, gint g, gint b, gint a);
GdkPixbuf * pixbuf_render_text(GtkWidget *da, const char *str, int point_size, int r, int g, int b, 
		int *w, int *h);

#endif /*__PIXBUF_UTIL_H__ */
