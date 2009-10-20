/* vi: set sw=4 ts=4: */
/*
 * desktop-reminder.h
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

#ifndef __DESKTOP_REMINDER_H__
#define __DESKTOP_REMINDER_H__  1

#include <glib-object.h>
#include <gtk/gtk.h>
#include "bacon-message-connection.h"

G_BEGIN_DECLS

#define DESKTOP_REMINDER_TYPE (desktop_reminder_get_type ())
#define DESKTOP_REMINDER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), DESKTOP_REMINDER_TYPE, DesktopReminder))
#define DESKTOP_REMINDER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), DESKTOP_REMINDER_TYPE, DesktopReminderClass))
#define IS_DESKTOP_REMINDER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), DESKTOP_REMINDER_TYPE))
#define IS_DESKTOP_REMINDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), DESKTOP_REMINDER_TYPE))
#define DESKTOP_REMINDER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), DESKTOP_REMINDER_TYPE, DesktopReminderClass))

typedef struct _DesktopReminderPrivate DesktopReminderPrivate;

typedef struct {
	GtkWindow parent;

	BaconMessageConnection *conn;
	DesktopReminderPrivate *priv;
} DesktopReminder;

typedef struct {
	GtkWindowClass parent;
} DesktopReminderClass;


GType desktop_reminder_get_type() G_GNUC_CONST;
DesktopReminder * desktop_reminder_new();
gboolean desktop_reminder_load_config(DesktopReminder *dr, const char *filename);
void desktop_reminder_display_message(DesktopReminder *dr, const char *msg);
gboolean desktop_reminder_is_show(DesktopReminder *dr);
void desktop_reminder_set_color(DesktopReminder *dr, int red, int green, int blue);
void desktop_reminder_set_font(DesktopReminder *dr, const char *font);
void desktop_reminder_move(DesktopReminder *dr, int x, int y);

G_END_DECLS

#endif /*__DESKTOP_REMINDER_H__ */
