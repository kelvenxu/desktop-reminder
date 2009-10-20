/* vi: set sw=4 ts=4: */
/*
 * desktop-remainer.c
 *
 * This file is part of ________.
 *
 * Copyright (C) 2009 - kelvenxu <kelvenxu@gmail.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 * */

#include <gtk/gtk.h>

#define TEXT_LEN 1024

GdkPixbuf *pixbuf_render_text(GtkWidget *da, const char *str, int point_size, int r, int g, int b, 
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
	g_object_unref(G_OBJECT(layout));

	*w = width;
	*h = height;
	return pixbuf;
}

gboolean win_destroy_cb(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	gtk_main_quit();
}

typedef struct _DesktopReminder
{
	GtkWidget *win;
	GtkWidget *da;
	GdkPixbuf *pixbuf;

	int red;
	int green;
	int blue;

	int point_size;
} DesktopReminder;

gboolean timer_cb(DesktopReminder *reminder)
{
	int width;
	int height;
	static int n = 0;

	GdkPixmap *pixmap;
	GdkBitmap *mask;

	g_return_val_if_fail(reminder != NULL, TRUE);

	if(reminder->pixbuf)
		g_object_unref(reminder->pixbuf);

	if(n % 2 == 0)
	{
		const char *text = "Ubuntu - linux";
		reminder->pixbuf = pixbuf_render_text(reminder->da, text, 16, 200, 100, 0, &width, &height);
	}
	else
	{
		const char *text = "I love Ubuntu";
		reminder->pixbuf = pixbuf_render_text(reminder->da, text, 16, 200, 100, 0, &width, &height);
	}
	gtk_widget_set_size_request(reminder->da, width, height);
	gtk_widget_set_size_request(reminder->win, width, height);

	gdk_pixbuf_render_pixmap_and_mask(reminder->pixbuf, &pixmap, &mask, 100);
	gdk_window_shape_combine_mask(reminder->win->window, mask, 0, 0);

	++n;
	return TRUE;
}

int main(int argc, char *argv[])
{
	int point_size = 16;
	int width;
	int height;
	const char *text = "Hello, world!";

	gtk_init(&argc, &argv);
	DesktopReminder reminder;
	
	GtkWidget *win = gtk_window_new(GTK_WINDOW_POPUP);

	GtkWidget *da = gtk_drawing_area_new();

	gtk_container_add(GTK_CONTAINER(win), da);
	gtk_widget_realize(da);
	gtk_widget_show(da);

	GdkPixbuf *pixbuf = pixbuf_render_text(da, text, 16, 200, 100, 0, &width, &height);
	gtk_widget_set_size_request(da, width, height);
	gtk_widget_set_size_request(win, width, height);

	GdkPixmap *pixmap;
	GdkBitmap *mask;

	gdk_pixbuf_render_pixmap_and_mask(pixbuf, &pixmap, &mask, 100);
	gdk_window_shape_combine_mask(win->window, mask, 0, 0);

	g_signal_connect(G_OBJECT(win), "destroy", G_CALLBACK(win_destroy_cb), NULL);
	gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER_ALWAYS);
	gtk_widget_show_all(win);

	reminder.win = win;
	reminder.da = da;
	reminder.pixbuf = pixbuf;
	g_timeout_add_seconds(1, (GSourceFunc)timer_cb, &reminder);
	gtk_main();

	g_object_unref(pixbuf);
}
