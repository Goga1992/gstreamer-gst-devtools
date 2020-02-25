/* GStreamer
 * Copyright (C) 2013 Thibault Saunier <thibault.saunier@collabora.com>
 *
 * gst-validate-runner.c - Validate Runner class
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __GST_VALIDATE_SCENARIO_H__
#define __GST_VALIDATE_SCENARIO_H__

#include <glib.h>
#include <glib-object.h>

#include "gst-validate-types.h"
#include <gst/validate/gst-validate-runner.h>

G_BEGIN_DECLS

#define GST_TYPE_VALIDATE_SCENARIO            (gst_validate_scenario_get_type ())
#define GST_VALIDATE_SCENARIO(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_VALIDATE_SCENARIO, GstValidateScenario))
#define GST_VALIDATE_SCENARIO_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_VALIDATE_SCENARIO, GstValidateScenarioClass))
#define GST_IS_VALIDATE_SCENARIO(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_VALIDATE_SCENARIO))
#define GST_IS_VALIDATE_SCENARIO_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_VALIDATE_SCENARIO))
#define GST_VALIDATE_SCENARIO_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_VALIDATE_SCENARIO, GstValidateScenarioClass))

typedef struct _GstValidateScenarioPrivate GstValidateScenarioPrivate;
typedef struct _GstValidateActionParameter GstValidateActionParameter;

/**
 * GstValidateActionReturn:
 * GST_VALIDATE_EXECUTE_ACTION_ERROR:
 * GST_VALIDATE_EXECUTE_ACTION_OK:
 * GST_VALIDATE_EXECUTE_ACTION_ASYNC:
 * GST_VALIDATE_EXECUTE_ACTION_INTERLACED:
 * GST_VALIDATE_EXECUTE_ACTION_ERROR_REPORTED:
 */
typedef enum
{
  GST_VALIDATE_EXECUTE_ACTION_ERROR,
  GST_VALIDATE_EXECUTE_ACTION_OK,
  GST_VALIDATE_EXECUTE_ACTION_ASYNC,
  GST_VALIDATE_EXECUTE_ACTION_INTERLACED,
  GST_VALIDATE_EXECUTE_ACTION_ERROR_REPORTED,
  GST_VALIDATE_EXECUTE_ACTION_IN_PROGRESS,
  GST_VALIDATE_EXECUTE_ACTION_NONE,
} GstValidateActionReturn;

/* TODO 2.0 -- Make it an actual enum type */
#define GstValidateExecuteActionReturn gint

/**
 * GstValidateExecuteAction:
 * @scenario: The #GstValidateScenario from which the @action is executed
 * @action: The #GstValidateAction being executed
 *
 * A function that executes a #GstValidateAction
 *
 * Returns: a #GstValidateExecuteActionReturn
 */
typedef GstValidateExecuteActionReturn (*GstValidateExecuteAction) (GstValidateScenario * scenario, GstValidateAction * action);

/**
 * GstValidatePrepareAction:
 * @action: The #GstValidateAction to prepare before execution
 *
 * A function that prepares @action so it can be executed right after.
 * Most of the time this function is used to parse and set fields with
 * equations in the action structure.
 *
 * Returns: %TRUE if the action could be prepared and is ready to be run
 *          , %FALSE otherwise
 */
typedef gboolean (*GstValidatePrepareAction) (GstValidateAction * action);


typedef struct _GstValidateActionPrivate          GstValidateActionPrivate;

#define GST_VALIDATE_ACTION_LINENO(action) (action->ABI.abi.lineno)
#define GST_VALIDATE_ACTION_FILENAME(action) (action->ABI.abi.filename)

/**
 * GstValidateAction:
 * @type: The type of the #GstValidateAction, which is the name of the
 *        GstValidateActionType registered with
 *        #gst_validate_register_action_type
 * @name: The name of the action, set from the user in the scenario
 * @structure: the #GstStructure defining the action
 * @scenario: The scenario for this action. This is not thread-safe
 * and should be accessed exclusively from the main thread.
 * If you need to access it from another thread use the
 * #gst_validate_action_get_scenario method
 *
 * The GstValidateAction defined to be executed as part of a scenario
 *
 * Only access it from the default main context.
 */
struct _GstValidateAction
{
  GstMiniObject          mini_object;

  /*< public > */
  const gchar *type;
  const gchar *name;
  GstStructure *structure;

  /* < private > */
  guint action_number;
  gint repeat;
  GstClockTime playback_time;

  GstValidateActionPrivate *priv;

  union {
    gpointer _gst_reserved[GST_PADDING_LARGE - 1]; /* ->priv */
    struct {
      gint lineno;
      gchar *filename;
    } abi;
  } ABI;
};

GST_VALIDATE_API
void                  gst_validate_action_set_done     (GstValidateAction *action);
GST_VALIDATE_API
GstValidateScenario * gst_validate_action_get_scenario (GstValidateAction *action);
GST_VALIDATE_API
GstValidateAction   * gst_validate_action_new          (GstValidateScenario * scenario,
                                                        GstValidateActionType * action_type,
                                                        GstStructure *structure,
                                                        gboolean add_to_lists);
GST_VALIDATE_API
GstValidateAction* gst_validate_action_ref             (GstValidateAction * action);
GST_VALIDATE_API
void gst_validate_action_unref                         (GstValidateAction * action);

#define GST_TYPE_VALIDATE_ACTION            (gst_validate_action_get_type ())
#define GST_IS_VALIDATE_ACTION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_VALIDATE_ACTION))
#define GST_VALIDATE_ACTION_GET_TYPE(obj)   ((GstValidateActionType*)gst_validate_get_action_type(((GstValidateAction*)obj)->type))
GST_VALIDATE_API
GType gst_validate_action_get_type (void);

/**
 * GstValidateActionTypeFlags:
 * @GST_VALIDATE_ACTION_TYPE_NONE: No special flag
 * @GST_VALIDATE_ACTION_TYPE_CONFIG: The action is a config
 * @GST_VALIDATE_ACTION_TYPE_ASYNC: The action can be executed ASYNC
 * @GST_VALIDATE_ACTION_TYPE_INTERLACED: The action will be executed async
 *                                       but without blocking further actions
 *                                       to be executed
 * @GST_VALIDATE_ACTION_TYPE_CAN_EXECUTE_ON_ADDITION: The action will be executed on 'element-added'
 *                                                 for a particular element type if no playback-time
 *                                                 is specified
 * @GST_VALIDATE_ACTION_TYPE_NEEDS_CLOCK: The pipeline will need to be synchronized with the clock
 *                                        for that action type to be used.
 * @GST_VALIDATE_ACTION_TYPE_NO_EXECUTION_NOT_FATAL: Do not consider the non execution of the action
 *                                                   as a fatal error.
 * @GST_VALIDATE_ACTION_TYPE_CAN_BE_OPTIONAL: The action can use the 'optional' keyword. Such action
 *                                            instances will have the #GST_VALIDATE_ACTION_TYPE_NO_EXECUTION_NOT_FATAL
 *                                            flag set and won't be considered as fatal if they fail.
 * @GST_VALIDATE_ACTION_TYPE_HANDLED_IN_CONFIG: The action can be used in config files even if it is not strictly a config
 *                                              action (ie. it needs a scenario to run).
 */
typedef enum
{
    GST_VALIDATE_ACTION_TYPE_NONE = 0,
    GST_VALIDATE_ACTION_TYPE_CONFIG = 1 << 1,
    GST_VALIDATE_ACTION_TYPE_ASYNC = 1 << 2,
    GST_VALIDATE_ACTION_TYPE_INTERLACED = 1 << 3,
    GST_VALIDATE_ACTION_TYPE_CAN_EXECUTE_ON_ADDITION = 1 << 4,
    GST_VALIDATE_ACTION_TYPE_NEEDS_CLOCK = 1 << 5,
    GST_VALIDATE_ACTION_TYPE_NO_EXECUTION_NOT_FATAL = 1 << 6,
    GST_VALIDATE_ACTION_TYPE_CAN_BE_OPTIONAL = 1 << 7,
    GST_VALIDATE_ACTION_TYPE_DOESNT_NEED_PIPELINE = 1 << 8,
    GST_VALIDATE_ACTION_TYPE_HANDLED_IN_CONFIG = 1 << 9,
} GstValidateActionTypeFlags;

/**
 * GstValidateActionType:
 * @name: The name of the new action type to add
 * @implementer_namespace: The namespace of the implementer of the action type
 * @execute: The function to be called to execute the action
 * @parameters: (allow-none) (array zero-terminated=1) (element-type GstValidateActionParameter): The #GstValidateActionParameter usable as parameter of the type
 * @description: A description of the new type
 * @flags: The flags of the action type
 */
struct _GstValidateActionType
{
  GstMiniObject          mini_object;

  gchar *name;
  gchar *implementer_namespace;

  GstValidatePrepareAction prepare;
  GstValidateExecuteAction execute;

  GstValidateActionParameter *parameters;

  gchar *description;
  GstValidateActionTypeFlags flags;

  GstRank rank;

  GstValidateActionType *overriden_type;

  /*< private >*/
  gpointer _gst_reserved[GST_PADDING_LARGE - sizeof (GstRank) - 1];
};

#define GST_TYPE_VALIDATE_ACTION_TYPE       (gst_validate_action_type_get_type ())
#define GST_IS_VALIDATE_ACTION_TYPE(obj)    (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_VALIDATE_ACTION_TYPE))
#define GST_VALIDATE_ACTION_TYPE(obj)       ((GstValidateActionType*) obj)
GST_VALIDATE_API
GType gst_validate_action_type_get_type     (void);

GST_VALIDATE_API
gboolean gst_validate_print_action_types (const gchar ** wanted_types, gint num_wanted_types);

/**
 * GstValidateActionParameter:
 * @name: The name of the parameter
 * @description: The description of the parameter
 * @mandatory: Whether the parameter is mandatory for
 *             a specific action type
 * @types: The types the parameter can take described as a
 *         string. It can be precisely describing how the typing works
 *         using '\n' between the various acceptable types.
 *         NOTE: The types should end with `(GstClockTime)` if its final
 *         type is a GstClockTime, this way it will be processed when preparing
 *         the actions.
 * @possible_variables: The name of the variables that can be
 *                      used to compute the value of the parameter.
 *                      For example for the start value of a seek
 *                      action, we will accept to take 'duration'
 *                      which will be replace by the total duration
 *                      of the stream on which the action is executed.
 * @def: The default value of a parameter as a string, should be %NULL
 *       for mandatory streams.
 */
struct _GstValidateActionParameter
{
  const gchar  *name;
  const gchar  *description;
  gboolean     mandatory;
  const gchar  *types;
  const gchar  *possible_variables;
  const gchar  *def;

  /*< private >*/
  gpointer     _gst_reserved[GST_PADDING];
};

struct _GstValidateScenarioClass
{
  GstObjectClass parent_class;

  /*< public >*/
  /*< private >*/
  gpointer _gst_reserved[GST_PADDING];
};

/**
 * GstValidateScenario:
 */
struct _GstValidateScenario
{
  GstObject parent;

  /*< public >*/
  GstStructure *description;

  /*< private >*/
  GstValidateScenarioPrivate *priv;

  union {
    gpointer _gst_reserved[GST_PADDING];
    struct {
      GMutex eos_handling_lock;
    } abi;
  } ABI;
};

/* Some actions may trigger EOS during their execution. Unlocked this
 * could cause a race condition as the main thread may terminate the test
 * in response to the EOS message in the bus while the action is still
 * going in a different thread.
 * To avoid this, the handling of the EOS message is protected with this
 * lock. Actions expecting to cause an EOS can hold the lock for their
 * duration so that they are guaranteed to finish before the EOS
 * terminates the test. */
#define GST_VALIDATE_SCENARIO_EOS_HANDLING_LOCK(scenario) (g_mutex_lock(&(scenario)->ABI.abi.eos_handling_lock))
#define GST_VALIDATE_SCENARIO_EOS_HANDLING_UNLOCK(scenario) (g_mutex_unlock(&(scenario)->ABI.abi.eos_handling_lock))

GST_VALIDATE_API
GType gst_validate_scenario_get_type (void);

GST_VALIDATE_API
GstValidateScenario * gst_validate_scenario_factory_create (GstValidateRunner *runner,
                                                GstElement *pipeline,
                                                const gchar *scenario_name);
GST_VALIDATE_API gboolean
gst_validate_list_scenarios       (gchar **scenarios,
                                   gint num_scenarios,
                                   gchar * output_file);

GST_VALIDATE_API GstValidateActionType *
gst_validate_get_action_type           (const gchar *type_name);

GST_VALIDATE_API GstValidateActionType *
gst_validate_register_action_type      (const gchar *type_name,
                                        const gchar *implementer_namespace,
                                        GstValidateExecuteAction function,
                                        GstValidateActionParameter * parameters,
                                        const gchar *description,
                                        GstValidateActionTypeFlags flags);

GST_VALIDATE_API GstValidateActionType *
gst_validate_register_action_type_dynamic (GstPlugin *plugin,
                                           const gchar * type_name,
                                           GstRank rank,
                                           GstValidateExecuteAction function,
                                           GstValidateActionParameter * parameters,
                                           const gchar * description,
                                           GstValidateActionTypeFlags flags);


GST_VALIDATE_API
gboolean gst_validate_action_get_clocktime (GstValidateScenario * scenario,
                                            GstValidateAction *action,
                                            const gchar * name,
                                            GstClockTime * retval);

GST_VALIDATE_API GstValidateExecuteActionReturn
gst_validate_scenario_execute_seek         (GstValidateScenario *scenario,
                                             GstValidateAction *action,
                                             gdouble rate,
                                             GstFormat format,
                                             GstSeekFlags flags,
                                             GstSeekType start_type,
                                             GstClockTime start,
                                             GstSeekType stop_type,
                                             GstClockTime stop);

GST_VALIDATE_API GList *
gst_validate_scenario_get_actions          (GstValidateScenario *scenario);
GST_VALIDATE_API GstValidateExecuteActionReturn
gst_validate_execute_action                 (GstValidateActionType * action_type,
                                             GstValidateAction * action);

GST_VALIDATE_API GstState
gst_validate_scenario_get_target_state     (GstValidateScenario *scenario);

GST_VALIDATE_API GstElement *
gst_validate_scenario_get_pipeline         (GstValidateScenario * scenario);

GST_VALIDATE_API
void gst_validate_scenario_deinit          (void);

G_END_DECLS

#endif /* __GST_VALIDATE_SCENARIOS__ */
