/* GStreamer
 *
 * Copyright (C) 2018-2019 Igalia S.L.
 * Copyright (C) 2018 Metrological Group B.V.
 *  Author: Alicia Boya García <aboya@igalia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
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

#ifndef __GST_VALIDATE_FLOW_FORMATTING_H__
#define __GST_VALIDATE_FLOW_FORMATTING_H__

#include <gst/gst.h>

void format_time(gchar* dest_str, guint64 time);

gchar* validate_flow_format_segment (const GstSegment *segment);

gchar* validate_flow_format_caps (const GstCaps* caps, const gchar * const *keys_to_print);

gchar* validate_flow_format_buffer (GstBuffer *buffer, gboolean add_checksum);

gchar* validate_flow_format_event (GstEvent *event, const gchar * const *caps_properties, GstStructure *ignored_event_fields);

#endif // __GST_VALIDATE_FLOW_FORMATTING_H__
