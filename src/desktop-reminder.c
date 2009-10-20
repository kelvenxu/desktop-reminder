/* vi: set sw=4 ts=4: */
/*
 * desktop-reminder.c
 *
 * This file is part of desktop-reminder.
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

#include "desktop-reminder.h"
#include "pixbuf_util.h"
#include <glib/gi18n.h>

G_DEFINE_TYPE(DesktopReminder, desktop_reminder, GTK_TYPE_WINDOW);

#define DESKTOP_REMINDER_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE ((o), DESKTOP_REMINDER_TYPE, DesktopReminderPrivate))

struct _DesktopReminderPrivate 
{
	GtkWidget *da;

	int width;
	int height;

	int x;
	int y;

	/* color */
	int red;
	int green;
	int blue;

	gchar *cfg;
	GKeyFile *key_file;

	int point_size;
	char *font;
	char *msg;

	gboolean is_show;
};


static void
desktop_reminder_dispose (DesktopReminder *self)
{
}

static void
desktop_reminder_finalize (DesktopReminder *self)
{	
	DesktopReminderPrivate *priv;
	priv = DESKTOP_REMINDER_GET_PRIVATE (self);

	if(priv->key_file)
	{
		g_key_file_free(priv->key_file);
		priv->key_file = NULL;
	}
	if(priv->cfg)
	{
		g_free(priv->cfg);
		priv->cfg = NULL;
	}
	if(priv->msg)
	{
		g_free(priv->msg);
		priv->msg = NULL;
	}
	if(priv->font)
	{
		g_free(priv->font);
		priv->font = NULL;
	}
	
}

static void
desktop_reminder_init (DesktopReminder *self)
{
	DesktopReminderPrivate *priv;

	priv = DESKTOP_REMINDER_GET_PRIVATE (self);

	priv->width = 120;
	priv->height = 64;
	priv->point_size = 12;

	priv->red = 255;
	priv->green = 255;
	priv->blue = 255;
	priv->is_show = TRUE;

	self->conn = NULL;
}

static void
desktop_reminder_class_init (DesktopReminderClass *self_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS (self_class);

	g_type_class_add_private (self_class, sizeof (DesktopReminderPrivate));
	object_class->dispose = (void (*) (GObject *object)) desktop_reminder_dispose;
	object_class->finalize = (void (*) (GObject *object)) desktop_reminder_finalize;
}

#if 0
GType
desktop_reminder_get_type()
{
	static GType desktop_reminder_type;

	if (!desktop_reminder_type) {
		static const GTypeInfo object_info = {
			sizeof (DesktopReminderClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) desktop_reminder_class_init,
			(GClassFinalizeFunc) NULL,
			NULL,			/* class_data */
			sizeof (DesktopReminder),
			0,			/* n_preallocs */
			(GInstanceInitFunc) desktop_reminder_init,
			NULL			/* value_table */
		};

		desktop_reminder_type = g_type_register_static(GTK_TYPE_WINDOW, "DesktopReminder", &object_info, 0);
	}

	return desktop_reminder_type;
}
#endif

static gboolean 
win_destroy_cb(GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
	gtk_main_quit();

	return FALSE;
}

DesktopReminder *
desktop_reminder_new()
{
	DesktopReminder *dr;
	DesktopReminderPrivate *priv = NULL;
	const gchar *text = _("Thanks to use Desktop Reminder!");

	dr = g_object_new(DESKTOP_REMINDER_TYPE, "type", GTK_WINDOW_POPUP, NULL);

	priv = DESKTOP_REMINDER_GET_PRIVATE(dr);
	priv->da = gtk_drawing_area_new();

	gtk_container_add(GTK_CONTAINER(dr), priv->da);
	gtk_widget_realize(priv->da);
	gtk_widget_show(priv->da);

	//desktop_reminder_load_config(dr, NULL);
	desktop_reminder_display_message(dr, text);

	g_signal_connect(G_OBJECT(dr), "destroy", G_CALLBACK(win_destroy_cb), NULL);

	gtk_window_set_position(GTK_WINDOW(dr), GTK_WIN_POS_CENTER_ALWAYS);
	priv->cfg = g_build_filename(g_getenv("HOME"), ".desktop-reminder/config", NULL);

	return dr;
}

gboolean
desktop_reminder_load_config(DesktopReminder *dr, const char *filename)
{
	GError *error = NULL;
	DesktopReminderPrivate *priv;
	
	g_return_val_if_fail(IS_DESKTOP_REMINDER(dr), FALSE);

	priv = DESKTOP_REMINDER_GET_PRIVATE(dr);

	priv->key_file = g_key_file_new();
	if(filename)
	{
		g_key_file_load_from_file(priv->key_file, filename, G_KEY_FILE_NONE, &error);
	}
	else
	{
		g_key_file_load_from_file(priv->key_file, priv->cfg, G_KEY_FILE_NONE, &error);
	}
	if(error)
	{
		return FALSE;
	}

	priv->is_show = g_key_file_get_boolean(priv->key_file, "default", "display", NULL);

	if(priv->font)
	{
		g_free(priv->font);
		priv->font = NULL;
	}
	priv->font = g_key_file_get_string(priv->key_file, "default", "font", NULL);

	if(priv->msg)
	{
		g_free(priv->msg);
		priv->msg = NULL;
	}
	priv->msg = g_key_file_get_string(priv->key_file, "default", "message", NULL);

	priv->red = g_key_file_get_integer(priv->key_file, "color", "red", NULL);
	priv->green = g_key_file_get_integer(priv->key_file, "color", "green", NULL);
	priv->blue = g_key_file_get_integer(priv->key_file, "color", "blue", NULL);

	g_key_file_free(priv->key_file);
	priv->key_file = NULL;

	return TRUE;
}

void
desktop_reminder_display_message(DesktopReminder *dr, const char *msg)
{
	GdkPixmap *pixmap;
	GdkBitmap *mask;
	GdkPixbuf *pixbuf;
	int width;
	int height;
	const char *p = NULL;

	DesktopReminderPrivate *priv;

	g_return_if_fail(IS_DESKTOP_REMINDER(dr));
	
	priv = DESKTOP_REMINDER_GET_PRIVATE(dr);

	if(msg)
		p = msg;
	else
		p = priv->msg;

	// FIXME: Can't change color
	pixbuf = pixbuf_render_text(priv->da, p, priv->point_size, priv->red, priv->green, priv->blue, &width, &height);

	gtk_widget_set_size_request(priv->da, width, height);
	gtk_widget_set_size_request(GTK_WIDGET(dr), width, height);

	GdkGC *gc = gdk_gc_new(priv->da->window);
	gdk_pixbuf_render_pixmap_and_mask(pixbuf, &pixmap, &mask, 100);
	gdk_draw_drawable(priv->da->window, gc, pixmap, 0, 0, 0, 0, width, height);
	gdk_window_shape_combine_mask(GTK_WIDGET(dr)->window, mask, 0, 0);

	priv->width = width;
	priv->height = height;

	g_object_unref(gc);
}

gboolean 
desktop_reminder_is_show(DesktopReminder *dr)
{
	DesktopReminderPrivate *priv;

	g_return_val_if_fail(IS_DESKTOP_REMINDER(dr), FALSE);
	
	priv = DESKTOP_REMINDER_GET_PRIVATE(dr);
	return priv->is_show;
}

void
desktop_reminder_set_color(DesktopReminder *dr, int red, int green, int blue)
{
	DesktopReminderPrivate *priv;

	g_return_if_fail(IS_DESKTOP_REMINDER(dr));
	
	priv = DESKTOP_REMINDER_GET_PRIVATE(dr);

	priv->red = red;
	priv->green = green;
	priv->blue = blue;
}

void
desktop_reminder_set_font(DesktopReminder *dr, const char *font)
{
	DesktopReminderPrivate *priv;

	g_return_if_fail(IS_DESKTOP_REMINDER(dr));
	g_return_if_fail(font != NULL);
	
	priv = DESKTOP_REMINDER_GET_PRIVATE(dr);

	if(priv->font)
	{
		g_free(priv->font);
	}
	priv->font = g_strdup(font);
}

void 
desktop_reminder_move(DesktopReminder *dr, int x, int y)
{
	DesktopReminderPrivate *priv;

	g_return_if_fail(IS_DESKTOP_REMINDER(dr));
	
	priv = DESKTOP_REMINDER_GET_PRIVATE(dr);

	priv->x = x;
	priv->y = y;

	gtk_window_move(GTK_WINDOW(dr), x, y);
	gdk_window_move(GTK_WIDGET(dr)->window, x, y);
}


