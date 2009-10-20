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

typedef struct _ReminderPreferences
{
	GtkBuilder *builder;
	GtkWidget *dlg;
	GtkWidget *close;
	GtkWidget *display;
	GtkWidget *font;
	GtkWidget *color;
	GtkWidget *text;
	GKeyFile *key_file;
	gchar *cfg;
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
			size_t t = fwrite(p, 1, strlen(p), fp);
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

	printf("font: %s\n", font);
	g_key_file_set_string(rp->key_file, "default", "font", font);

}

static void
color_set_cb(GtkColorButton *button, ReminderPreferences *rp)
{
	GdkColor color;
	gtk_color_button_get_color(button, &color);

	g_key_file_set_integer(rp->key_file, "color", "red", color.red);
	g_key_file_set_integer(rp->key_file, "color", "green", color.green);
	g_key_file_set_integer(rp->key_file, "color", "blue", color.blue);

	printf("r: %d g: %d b: %d\n", color.red, color.green, color.blue);

}

static void
display_toggled_cb(GtkToggleButton *button, ReminderPreferences *rp)
{
	if(gtk_toggle_button_get_active(button))
	{
		g_key_file_set_boolean(rp->key_file, "default", "display", TRUE);
		//TODO: display message
	}
	else
	{
		g_key_file_set_boolean(rp->key_file, "default", "display", FALSE);
		//TODO: Hide message
	}

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
	gtk_color_button_set_color(GTK_COLOR_BUTTON(rp->color), &color);
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

	g_signal_connect(G_OBJECT(rp.dlg), "destroy", G_CALLBACK(dlg_destroy_cb), &rp);
	g_signal_connect(G_OBJECT(rp.close), "clicked", G_CALLBACK(close_clicked_cb), &rp);
	g_signal_connect(G_OBJECT(rp.font), "font-set", G_CALLBACK(font_set_cb), &rp);
	g_signal_connect(G_OBJECT(rp.color), "color-set", G_CALLBACK(color_set_cb), &rp);
	g_signal_connect(G_OBJECT(rp.display), "toggled", G_CALLBACK(display_toggled_cb), &rp);

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

	gtk_main();
	return 0;
}

