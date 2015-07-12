<?php
/**
 * @file
 * Function: filter_get_params()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Get the filter parameters from the GET request.
 *
 * The GET request is like: translations/search?lng=fr&words=xyz&limit=10
 */
function filter_get_params() {
  // Get a copy of the request parameters.
  $request = $_GET;

  // If the search filter is empty, try to search for strings
  // similar to the last one that was reviewed.
  if (!isset($request['lng'])
    and !isset($request['words'])
    and isset($_SESSION['btrClient']['last_sguid']))
    {
      $request['lng'] = bcl::get_translation_lng();
      $params['sguid'] = $_SESSION['btrClient']['last_sguid'];
    }

  // Get and check the language.
  $lng = isset($request['lng']) ? trim($request['lng']) : '';
  $languages = bcl::get_languages();
  $lng_codes = array_keys($languages);
  $params['lng'] = in_array($lng, $lng_codes) ? $lng : 'fr';

  // Number of results to be displayed.
  if (isset($request['limit'])) {
    $limit = (int) trim($request['limit']);
    list($limit_options, $default_limit) = bcl::filter_get_options('limit');
    $params['limit'] = in_array($limit, $limit_options) ? $limit : $default_limit;
  }
  // Page of results to be displayed.
  if (isset($request['page'])) {
    $params['page'] = (int) trim($request['page']);
  }

  // Search can be done either by similarity of l10n strings (natural search),
  // or by matching words according to a certain logic (boolean search).
  // Search can be performed either on l10n strings or on the translations.
  if (isset($request['mode'])) {
    $mode = $request['mode'];
    list($search_mode_options, $default_search_mode) = bcl::filter_get_options('mode');
    $params['mode'] = in_array($mode, $search_mode_options) ? $mode : $default_search_mode;
  }
  if (isset($request['words'])) {
    $params['words'] = $request['words'];
  }

  // Searching can be limited only to certain projects and/or origins.
  if (isset($request['project'])) {
    $params['project'] = trim($request['project']);
  }
  if (isset($request['origin'])) {
    $params['origin'] = trim($request['origin']);
  }

  // Limit search only to the strings touched (translated or voted)
  // by the current user.
  if (isset($request['only_mine']) && (int) $request['only_mine']) {
    $params['only_mine'] = 1;
  }

  // Limit search by the editing users (used by admins).
  if (isset($request['translated_by'])) {
    $params['translated_by'] = trim($request['translated_by']);
  }
  if (isset($request['voted_by'])) {
    $params['voted_by'] = trim($request['voted_by']);
  }

  // Limit by date of string, translation or voting (used by admins).
  if (isset($request['date_filter'])) {
    $date_filter = trim($request['date_filter']);
    list($date_filter_options, $default_date_filter) = bcl::filter_get_options('date_filter');
    $params['date_filter'] = in_array($date_filter, $date_filter_options) ? $date_filter : $default_date_filter;
  }

  // Get from_date.
  if (isset($request['from_date'])) {
    $from_date = trim($request['from_date']);
    if ($from_date != '') {
      $from_date = date('Y-m-d H:i:s', strtotime($from_date));
      $from_date = str_replace(' 00:00:00', '', $from_date);
    }
    $params['from_date'] = $from_date;
  }

  // Get to_date.
  if (isset($request['to_date'])) {
    $to_date = trim($request['to_date']);
    if ($to_date != '') {
      $to_date = date('Y-m-d H:i:s', strtotime($to_date));
      $to_date = str_replace(' 00:00:00', '', $to_date);
    }
    $params['to_date'] = $to_date;
  }

  // List all the strings of a project, or only the translated/untranslated strings.
  if (isset($request['list_mode'])) {
    $list_mode = $request['list_mode'];
    list($list_mode_options, $default_list_mode) = bcl::filter_get_options('list_mode');
    $params['list_mode'] = in_array($list_mode, $list_mode_options) ? $list_mode : $default_list_mode;
  }
  return $params;
}
