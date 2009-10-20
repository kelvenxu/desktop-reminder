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

#include "config.h"
#include <gtk/gtk.h>
#include <string.h>
#include "desktop-reminder.h"
#include "bacon-message-connection.h"

static void
message_connection_receive_cb(const char *msg, DesktopReminder *dr)
{
	printf("msg: %s\n", msg);
	if(strcmp(msg, "quit") == 0)
	{
		bacon_message_connection_free(dr->conn);
		dr->conn = NULL;
		g_object_unref(dr);
		gtk_main_quit();
	}
}

static gboolean 
timer_cb(DesktopReminder *reminder)
{
	static int n = 0;

	g_return_val_if_fail(reminder != NULL, TRUE);

	if(n % 2 == 0)
	{
		const char *text = "Ubuntu - linux";
		desktop_reminder_display_message(reminder, text);
	}
	else
	{
		const char *text = "全心全意为人民服务";
		desktop_reminder_display_message(reminder, text);
	}

	//desktop_reminder_move(reminder, 200, n);

	++n;
	return TRUE;
}

int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);

	DesktopReminder *reminder = desktop_reminder_new();
	reminder->conn = bacon_message_connection_new(GETTEXT_PACKAGE);
	if(!bacon_message_connection_get_is_server(reminder->conn))
	{
		bacon_message_connection_free(reminder->conn);
		reminder->conn = NULL;
		g_object_unref(reminder);
		return 0;
	}
	
	bacon_message_connection_set_callback(reminder->conn, 
			(BaconMessageReceivedFunc)message_connection_receive_cb, reminder);

	desktop_reminder_load_config(reminder, NULL);
	if(!desktop_reminder_is_show(reminder))
	{
		bacon_message_connection_free(reminder->conn);
		reminder->conn = NULL;
		g_object_unref(reminder);
		return 0;
	}

	gtk_widget_show_all(GTK_WIDGET(reminder));
	g_timeout_add_seconds(1, (GSourceFunc)timer_cb, reminder);
	gtk_main();

	return 0;
}

