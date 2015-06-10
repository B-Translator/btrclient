<?php
/**
 * @file
 * Function: filter_get_query()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Converts the array of form values into a suitable query (for redirect).
 *
 * All non-string values are converted to string representations.
 * If a form value is equal to the default value, it is removed (unset)
 * from the array.
 */
function filter_get_query($form_values) {
  $query = array();

  // Get lng.
  $languages = bcl::get_languages();
  $lng_codes = array_keys($languages);
  $lng = trim($form_values['lng']);
  if (in_array($lng, $lng_codes)) {
    $query['lng'] = $lng;
  }

  // Get project and origin.
  if (trim($form_values['project']) != '') {
    $query['project'] = $form_values['project'];
  }
  if (trim($form_values['origin']) != '') {
    $query['origin'] = $form_values['origin'];
  }

  if (bcl::user_is_authenticated()) {
    // Get only_mine.
    if ($form_values['only_mine'] == 1) {
      $query['only_mine'] = '1';
    }
    else {
      // Get translated_by, voted_by.
      if (trim($form_values['translated_by']) != '') {
        $query['translated_by'] = $form_values['translated_by'];
      }
      if (trim($form_values['voted_by']) != '') {
        $query['voted_by'] = $form_values['voted_by'];
      }
    }

    // Get date_filter.
    list($date_filter_options, $default_date_filter) = _btrClient_get_filter_options('date_filter');
    if (in_array($form_values['date_filter'], $date_filter_options) && $form_values['date_filter'] != $default_date_filter) {
      $query['date_filter'] = $form_values['date_filter'];
    }

    // Get from_date.
    if (trim($form_values['from_date']) != '') {
      $query['from_date'] = $form_values['from_date'];
    }

    // Get to_date.
    if (trim($form_values['to_date']) != '') {
      $query['to_date'] = $form_values['to_date'];
    }
  }

  // Get limit.
  list($limit_options, $default_limit) = _btrClient_get_filter_options('limit');
  if (in_array($form_values['limit'], $limit_options) && $form_values['limit'] != $default_limit) {
    $query['limit'] = $form_values['limit'];
  }

  // Get search mode and words.
  list($search_mode_options, $default_search_mode) = _btrClient_get_filter_options('mode');
  if (in_array($form_values['mode'], $search_mode_options) && $form_values['mode'] != $default_search_mode) {
    $query['mode'] = $form_values['mode'];
  }
  if (trim($form_values['words']) != '') {
    $query['words'] = $form_values['words'];
  }

  return $query;
}
