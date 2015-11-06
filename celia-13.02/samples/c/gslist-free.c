/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/. 
 */

/* 
 * MT safe
 */

/* Modified to pass CELIA analysis. */

// MS: load the local files
//#include "config.h"
//#include "glib.h"
//#include "galias.h"

#include "gslist.h"

/**
 * g_slist_free:
 * @list: a #GSList
 *
 * Frees all of the memory used by a #GSList.
 * The freed elements are returned to the slice allocator.
 */

/*@ requires acyclic(xlist);
 */
void
g_slist_free(GSList *xlist) {
    // g_slice_free_chain (GSList, xlist, next);
    GSList *l, *tmp;
    l = xlist;
    tmp = NULL;
    while (l != NULL) {
        tmp = l->next;
        free(l);
        l = NULL;
        l = tmp;
        tmp = NULL;
    }
}

