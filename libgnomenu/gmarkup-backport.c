#include <glib.h>
#if GLIB_MINOR_VERSION < 16
#include "gmarkup-backport.h"
static gboolean
g_markup_parse_boolean (const char  *string,
                        gboolean    *value)
{
  char const * const falses[] = { "false", "f", "no", "n", "0" };
  char const * const trues[] = { "true", "t", "yes", "y", "1" };
  int i;

  for (i = 0; i < G_N_ELEMENTS (falses); i++)
    {
      if (g_ascii_strcasecmp (string, falses[i]) == 0)
        {
          if (value != NULL)
            *value = FALSE;

          return TRUE;
        }
    }

  for (i = 0; i < G_N_ELEMENTS (trues); i++)
    {
      if (g_ascii_strcasecmp (string, trues[i]) == 0)
        {
          if (value != NULL)
            *value = TRUE;

          return TRUE;
        }
    }

  return FALSE;
}

/**
 * GMarkupCollectType:
 * @G_MARKUP_COLLECT_INVALID: used to terminate the list of attributes
 *                            to collect.
 * @G_MARKUP_COLLECT_STRING: collect the string pointer directly from
 *                           the attribute_values[] array.  Expects a
 *                           parameter of type (const char **).  If
 *                           %G_MARKUP_COLLECT_OPTIONAL is specified
 *                           and the attribute isn't present then the
 *                           pointer will be set to %NULL.
 * @G_MARKUP_COLLECT_STRDUP: as with %G_MARKUP_COLLECT_STRING, but
 *                           expects a paramter of type (char **) and
 *                           g_strdup()s the returned pointer.  The
 *                           pointer must be freed with g_free().
 * @G_MARKUP_COLLECT_BOOLEAN: expects a parameter of type (gboolean *)
 *                            and parses the attribute value as a
 *                            boolean.  Sets %FALSE if the attribute
 *                            isn't present.  Valid boolean values
 *                            consist of (case insensitive) "false",
 *                            "f", "no", "n", "0" and "true", "t",
 *                            "yes", "y", "1".
 * @G_MARKUP_COLLECT_TRISTATE: as with %G_MARKUP_COLLECT_BOOLEAN, but
 *                             in the case of a missing attribute a
 *                             value is set that compares equal to
 *                             neither %FALSE nor %TRUE.
 *                             G_MARKUP_COLLECT_OPTIONAL is implied.
 * @G_MARKUP_COLLECT_OPTIONAL: can be bitwise ORed with the other
 *                             fields.  If present, allows the
 *                             attribute not to appear.  A default
 *                             value is set depending on what value
 *                             type is used.
 *
 * A mixed enumerated type and flags field.  You must specify one type
 * (string, strdup, boolean, tristate).  Additionally, you may
 * optionally bitwise OR the type with the flag
 * %G_MARKUP_COLLECT_OPTIONAL.
 *
 * It is likely that this enum will be extended in the future to
 * support other types.
 **/

/**
 * g_markup_collect_attributes:
 * @element_name: the current tag name
 * @attribute_names: the attribute names
 * @attribute_values: the attribute values
 * @error: a pointer to a #GError or %NULL
 * @first_type: the #GMarkupCollectType of the
 *              first attribute
 * @first_attr: the name of the first attribute
 * @...: a pointer to the storage location of the
 *       first attribute (or %NULL), followed by
 *       more types names and pointers, ending
 *       with %G_MARKUP_COLLECT_INVALID.
 * 
 * Collects the attributes of the element from the
 * data passed to the #GMarkupParser start_element
 * function, dealing with common error conditions
 * and supporting boolean values.
 *
 * This utility function is not required to write
 * a parser but can save a lot of typing.
 *
 * The @element_name, @attribute_names,
 * @attribute_values and @error parameters passed
 * to the start_element callback should be passed
 * unmodified to this function.
 *
 * Following these arguments is a list of
 * "supported" attributes to collect.  It is an
 * error to specify multiple attributes with the
 * same name.  If any attribute not in the list
 * appears in the @attribute_names array then an
 * unknown attribute error will result.
 *
 * The #GMarkupCollectType field allows specifying
 * the type of collection to perform and if a
 * given attribute must appear or is optional.
 *
 * The attribute name is simply the name of the
 * attribute to collect.
 *
 * The pointer should be of the appropriate type
 * (see the descriptions under
 * #GMarkupCollectType) and may be %NULL in case a
 * particular attribute is to be allowed but
 * ignored.
 *
 * This function deals with issuing errors for missing attributes 
 * (of type %G_MARKUP_ERROR_MISSING_ATTRIBUTE), unknown attributes 
 * (of type %G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE) and duplicate 
 * attributes (of type %G_MARKUP_ERROR_INVALID_CONTENT) as well 
 * as parse errors for boolean-valued attributes (again of type
 * %G_MARKUP_ERROR_INVALID_CONTENT). In all of these cases %FALSE 
 * will be returned and @error will be set as appropriate.
 *
 * Return value: %TRUE if successful
 *
 * Since: 2.16
 **/
gboolean
g_markup_collect_attributes (const gchar         *element_name,
                             const gchar        **attribute_names,
                             const gchar        **attribute_values,
                             GError             **error,
                             GMarkupCollectType   first_type,
                             const gchar         *first_attr,
                             ...)
{
  GMarkupCollectType type;
  const gchar *attr;
  guint64 collected;
  int written;
  va_list ap;
  int i;

  type = first_type;
  attr = first_attr;
  collected = 0;
  written = 0;

  va_start (ap, first_attr);
  while (type != G_MARKUP_COLLECT_INVALID)
    {
      gboolean mandatory;
      const gchar *value;

      mandatory = !(type & G_MARKUP_COLLECT_OPTIONAL);
      type &= (G_MARKUP_COLLECT_OPTIONAL - 1);

      /* tristate records a value != TRUE and != FALSE
       * for the case where the attribute is missing
       */
      if (type == G_MARKUP_COLLECT_TRISTATE)
        mandatory = FALSE;

      for (i = 0; attribute_names[i]; i++)
        if (i >= 40 || !(collected & (G_GUINT64_CONSTANT(1) << i)))
          if (!strcmp (attribute_names[i], attr))
            break;

      /* ISO C99 only promises that the user can pass up to 127 arguments.
       * Subtracting the first 4 arguments plus the final NULL and dividing
       * by 3 arguments per collected attribute, we are left with a maximum
       * number of supported attributes of (127 - 5) / 3 = 40.
       *
       * In reality, nobody is ever going to call us with anywhere close to
       * 40 attributes to collect, so it is safe to assume that if i > 40
       * then the user has given some invalid or repeated arguments.  These
       * problems will be caught and reported at the end of the function.
       *
       * We know at this point that we have an error, but we don't know
       * what error it is, so just continue...
       */
      if (i < 40)
        collected |= (G_GUINT64_CONSTANT(1) << i);

      value = attribute_values[i];

      if (value == NULL && mandatory)
        {
          g_set_error (error, G_MARKUP_ERROR,
                       G_MARKUP_ERROR_MISSING_ATTRIBUTE,
                       "element '%s' requires attribute '%s'",
                       element_name, attr);

          va_end (ap);
          goto failure;
        }

      switch (type)
        {
        case G_MARKUP_COLLECT_STRING:
          {
            const char **str_ptr;

            str_ptr = va_arg (ap, const char **);

            if (str_ptr != NULL)
              *str_ptr = value;
          }
          break;

        case G_MARKUP_COLLECT_STRDUP:
          {
            char **str_ptr;

            str_ptr = va_arg (ap, char **);

            if (str_ptr != NULL)
              *str_ptr = g_strdup (value);
          }
          break;

        case G_MARKUP_COLLECT_BOOLEAN:
        case G_MARKUP_COLLECT_TRISTATE:
          if (value == NULL)
            {
              gboolean *bool_ptr;

              bool_ptr = va_arg (ap, gboolean *);

              if (bool_ptr != NULL)
                {
                  if (type == G_MARKUP_COLLECT_TRISTATE)
                    /* constructivists rejoice!
                     * neither false nor true...
                     */
                    *bool_ptr = -1;

                  else /* G_MARKUP_COLLECT_BOOLEAN */
                    *bool_ptr = FALSE;
                }
            }
          else
            {
              if (!g_markup_parse_boolean (value, va_arg (ap, gboolean *)))
                {
                  g_set_error (error, G_MARKUP_ERROR,
                               G_MARKUP_ERROR_INVALID_CONTENT,
                               "element '%s', attribute '%s', value '%s' "
                               "cannot be parsed as a boolean value",
                               element_name, attr, value);

                  va_end (ap);
                  goto failure;
                }
            }

          break;

        default:
          g_assert_not_reached ();
        }

      type = va_arg (ap, GMarkupCollectType);
      attr = va_arg (ap, const char *);
      written++;
    }
  va_end (ap);

  /* ensure we collected all the arguments */
  for (i = 0; attribute_names[i]; i++)
    if ((collected & (G_GUINT64_CONSTANT(1) << i)) == 0)
      {
        /* attribute not collected:  could be caused by two things.
         *
         * 1) it doesn't exist in our list of attributes
         * 2) it existed but was matched by a duplicate attribute earlier
         *
         * find out.
         */
        int j;

        for (j = 0; j < i; j++)
          if (strcmp (attribute_names[i], attribute_names[j]) == 0)
            /* duplicate! */
            break;

        /* j is now the first occurance of attribute_names[i] */
        if (i == j)
          g_set_error (error, G_MARKUP_ERROR,
                       G_MARKUP_ERROR_UNKNOWN_ATTRIBUTE,
                       "attribute '%s' invalid for element '%s'",
                       attribute_names[i], element_name);
        else
          g_set_error (error, G_MARKUP_ERROR,
                       G_MARKUP_ERROR_INVALID_CONTENT,
                       "attribute '%s' given multiple times for element '%s'",
                       attribute_names[i], element_name);

        goto failure;
      }

  return TRUE;

failure:
  /* replay the above to free allocations */
  type = first_type;
  attr = first_attr;

  va_start (ap, first_attr);
  while (type != G_MARKUP_COLLECT_INVALID)
    {
      gpointer ptr;

      ptr = va_arg (ap, gpointer);

      if (ptr == NULL)
        continue;

      switch (type & (G_MARKUP_COLLECT_OPTIONAL - 1))
        {
        case G_MARKUP_COLLECT_STRDUP:
          if (written)
            g_free (*(char **) ptr);

        case G_MARKUP_COLLECT_STRING:
          *(char **) ptr = NULL;
          break;

        case G_MARKUP_COLLECT_BOOLEAN:
          *(gboolean *) ptr = FALSE;
          break;

        case G_MARKUP_COLLECT_TRISTATE:
          *(gboolean *) ptr = -1;
          break;
        }

      type = va_arg (ap, GMarkupCollectType);
      attr = va_arg (ap, const char *);

      if (written)
        written--;
    }
  va_end (ap);

  return FALSE;
}
#endif
