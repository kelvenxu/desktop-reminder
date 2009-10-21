/* vi: set sw=4 ts=4: */
/*
 * main.c
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

#include "config.h"
#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../src/bacon-message-connection.h"

typedef struct _ReminderPreferences
{
	GtkBuilder *builder;
	GtkWidget *dlg;
	GtkWidget *close;
	GtkWidget *display;
	GtkWidget *font;
	GtkWidget *color;
	GtkWidget *text;
	GtkWidget *xposition;
	GtkWidget *yposition;
	GKeyFile *key_file;
	gchar *cfg;
	BaconMessageConnection *conn;
} ReminderPreferences;

static char *
reminder_interface_get_full_path(const char *name)
{
	char *filename;

#ifdef DESKTOP_REMINDER_RUN_IN_SOURCE_TREE
	/* Try the GtkBuilder file in the source tree first */
	filename = g_build_filename("..", "data", name, NULL);
	if(g_file_test(filename, G_FILE_TEST_EXISTS) == FALSE)
	{
		g_free (filename);
		/* Try the local file */
		filename = g_build_filename(DATADIR, "desktop-reminder", name, NULL);
		g_print("filename: %s\n", filename);

		if(g_file_test(filename, G_FILE_TEST_EXISTS) == FALSE)
		{
			g_free (filename);
			return NULL;
		}
	}
#else
	filename = g_build_filename (DATADIR, "desktop-reminder", name, NULL);
#endif

	return filename;
}

static void
reminder_preferences_free(ReminderPreferences *rp)
{
	if(!rp)
		return;

	if(rp->key_file)
	{
		g_key_file_free(rp->key_file);
		rp->key_file = NULL;
	}
	if(rp->cfg)
	{
		g_free(rp->cfg);
		rp->cfg = NULL;
	}
}

static void
reminder_preferences_save_config(ReminderPreferences *rp)
{
	gchar *p = g_key_file_to_data(rp->key_file, NULL, NULL);
	if(p)
	{
		FILE *fp = fopen(rp->cfg, "w");
		if(fp)
		{
			size_t t; 
			t = fwrite(p, 1, strlen(p), fp);
			fclose(fp);
		}

		g_free(p);
	}
}

static void
dlg_destroy_cb(GtkWidget *widget, ReminderPreferences *rp)
{
	if(rp)
	{
		reminder_preferences_save_config(rp);
		reminder_preferences_free(rp);
	}
	gtk_main_quit();
}

static void
close_clicked_cb(GtkButton *button, ReminderPreferences *rp)
{
	if(rp)
	{
		reminder_preferences_save_config(rp);
		reminder_preferences_free(rp);
	}
	gtk_main_quit();
}

static void
font_set_cb(GtkFontButton *button, ReminderPreferences *rp)
{
	const gchar *font = gtk_font_button_get_font_name(button);

	g_key_file_set_string(rp->key_file, "default", "font", font);
}

static void
color_set_cb(GtkColorButton *button, ReminderPreferences *rp)
{
	GdkColor color;
	gtk_color_button_get_color(button, &color);

	g_key_file_set_integer(rp->key_file, "color", "red", (int)((float)color.red / 65535.0 * 256.0));
	g_key_file_set_integer(rp->key_file, "color", "green", (int)((float)color.green / 65535.0 * 256.0));
	g_key_file_set_integer(rp->key_file, "color", "blue", (int)((float)color.blue / 65535.0 * 256.0));
}

static void
display_toggled_cb(GtkToggleButton *button, ReminderPreferences *rp)
{
	if(gtk_toggle_button_get_active(button))
	{
		g_key_file_set_boolean(rp->key_file, "default", "display", TRUE);

		if(0 == fork())
		{
			execlp("desktop-reminder", "desktop-reminder", NULL);
		}
	}
	else
	{
		g_key_file_set_boolean(rp->key_file, "default", "display", FALSE);
		//TODO: Hide message
		bacon_message_connection_send(rp->conn, "quit");
	}
}

static void
x_change_value_cb(GtkSpinButton *button, ReminderPreferences *rp)
{
	char msg[64];
	int x;
	x = gtk_spin_button_get_value_as_int(button);
	printf("x = %d\n", x);
	sprintf(msg, "move_x %d", x);
	bacon_message_connection_send(rp->conn, msg);
}

static void
y_change_value_cb(GtkSpinButton *button, ReminderPreferences *rp)
{
	char msg[64];
	int y;
	y = gtk_spin_button_get_value_as_int(button);
	printf("y = %d\n", y);
	sprintf(msg, "move_y %d", y);
	bacon_message_connection_send(rp->conn, msg);
}

static void
update_ui_from_config(ReminderPreferences *rp)
{
	if(!rp)
		return;

	if(!rp->key_file)
		return;

	GError *error = NULL;

	gchar *p = g_key_file_get_string(rp->key_file, "default", "font", &error);
	if(error)
	{
		g_print("error: %s\n", error->message);
	}
	gtk_font_button_set_font_name(GTK_FONT_BUTTON(rp->font), p);
	g_free(p);

	gboolean b = g_key_file_get_boolean(rp->key_file, "default", "display", &error);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rp->display), b);

	GdkColor color;
	color.red = g_key_file_get_integer(rp->key_file, "color", "red", &error);
	color.green = g_key_file_get_integer(rp->key_file, "color", "green", &error);
	color.blue = g_key_file_get_integer(rp->key_file, "color", "blue", &error);

	color.red = (int)((float)color.red / 256.0 * 65535.0);
	color.green = (int)((float)color.green / 256.0 * 65535.0);
	color.blue = (int)((float)color.blue / 256.0 * 65535.0);

	gtk_color_button_set_color(GTK_COLOR_BUTTON(rp->color), &color);
}

static gboolean 
timer_cb(ReminderPreferences *rp)
{
	rp->conn = bacon_message_connection_new(GETTEXT_PACKAGE);
	return FALSE;
}

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	ReminderPreferences rp;

	rp.builder = gtk_builder_new();
	gchar *filename = reminder_interface_get_full_path("reminder-preferences.ui");

	g_return_val_if_fail(filename != NULL, -1);

	GError *error = NULL;
	if(gtk_builder_add_from_file(rp.builder, filename, &error) == 0)
	{
		g_print("err: %s\n", error->message);
		g_free(filename);
		g_error_free(error);
		return -1;
	}

	g_free(filename);
	g_return_val_if_fail(GTK_IS_BUILDER(rp.builder), -1);

	rp.dlg = (GtkWidget*)gtk_builder_get_object(rp.builder, "reminder-preferences-dialog");
	rp.close = (GtkWidget*)gtk_builder_get_object(rp.builder, "reminder-preferences-close");
	rp.font = (GtkWidget*)gtk_builder_get_object(rp.builder, "fontbutton1");
	rp.color = (GtkWidget*)gtk_builder_get_object(rp.builder, "colorbutton1");
	rp.text = (GtkWidget*)gtk_builder_get_object(rp.builder, "textview1");
	rp.display = (GtkWidget*)gtk_builder_get_object(rp.builder, "display-reminder");
	rp.xposition = (GtkWidget*)gtk_builder_get_object(rp.builder, "position-x");
	rp.yposition = (GtkWidget*)gtk_builder_get_object(rp.builder, "position-y");


	g_signal_connect(G_OBJECT(rp.dlg), "destroy", G_CALLBACK(dlg_destroy_cb), &rp);
	g_signal_connect(G_OBJECT(rp.close), "clicked", G_CALLBACK(close_clicked_cb), &rp);
	g_signal_connect(G_OBJECT(rp.font), "font-set", G_CALLBACK(font_set_cb), &rp);
	g_signal_connect(G_OBJECT(rp.color), "color-set", G_CALLBACK(color_set_cb), &rp);
	g_signal_connect(G_OBJECT(rp.display), "toggled", G_CALLBACK(display_toggled_cb), &rp);

	g_signal_connect(G_OBJECT(rp.xposition), "change-value", G_CALLBACK(x_change_value_cb), &rp);
	g_signal_connect(G_OBJECT(rp.yposition), "change-value", G_CALLBACK(y_change_value_cb), &rp);

	g_signal_connect(G_OBJECT(rp.xposition), "value-changed", G_CALLBACK(x_change_value_cb), &rp);
	g_signal_connect(G_OBJECT(rp.yposition), "value-changed", G_CALLBACK(y_change_value_cb), &rp);

	GdkScreen *screen = gdk_screen_get_default();
	int width = gdk_screen_get_width(screen);
	int height = gdk_screen_get_height(screen);
	printf("width: %d height: %d\n", width, height);

	GtkAdjustment *x_adj = (GtkAdjustment*)gtk_adjustment_new(width / 2, 0, width, 1.0, 5.0, 5.0);
	GtkAdjustment *y_adj = (GtkAdjustment*)gtk_adjustment_new(height / 2, 0, height, 1.0, 5.0, 5.0);

	gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(rp.xposition), x_adj);
	gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(rp.yposition), y_adj);

	//gtk_spin_button_set_range(GTK_SPIN_BUTTON(rp.xposition), 0.0, (gdouble)width);
	//gtk_spin_button_set_range(GTK_SPIN_BUTTON(rp.yposition), 0.0, (gdouble)height);

	rp.key_file = g_key_file_new();
	rp.cfg = g_build_filename(g_getenv("HOME"), ".desktop-reminder/config", NULL);
	printf("cfg: %s\n", rp.cfg);
	g_key_file_load_from_file(rp.key_file, rp.cfg, G_KEY_FILE_KEEP_COMMENTS, &error);
	if(error)
	{
		g_print("g_key_file_load_from_file error: %s\n", error->message);
	}

	update_ui_from_config(&rp);

	gtk_widget_show(rp.dlg);
	
	rp.conn = bacon_message_connection_new(GETTEXT_PACKAGE);
	if(bacon_message_connection_get_is_server(rp.conn))
	{
		printf("is server\n");
		bacon_message_connection_free(rp.conn);
		if(0 == fork())
		{
			//system("desktop-reminder", "desktop-reminder", NULL);
			printf("execlp\n");
			execlp("desktop-reminder", "desktop-reminder", NULL);
			//execlp("desktop-reminder", NULL, NULL);
		}
		g_timeout_add_seconds(2, (GSourceFunc)timer_cb, &rp);
	}

	gtk_main();
	return 0;
}

