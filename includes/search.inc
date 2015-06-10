<?php
/**
 * @file
 * Translation view and editing pages for localization community.
 */

/**
 * Include utility and theme functions.
 */
require_once dirname(__FILE__) . '/search_form.inc';

/**
 * Menu callback: translations/search.
 *
 * Search strings and translations using various filters.
 *
 * If the request header 'Accept: application/json' is set, then
 * the output will be in JSON, otherwise it will be an HTML page.
 * For example:
 *   curl -k -i -H 'Accept: application/json' \
 *        'https://dev.l10n.org.xx/translations/search?lng=sq&words=space'
 */
function btrClient_search() {
  // Get the filter parameters from the GET request.
  $params = bcl::filter_get_params();

  // Get the strings that match the search params.
  $btr = bcl::user_is_authenticated() ?
    wsclient_service_load('btr') :
    wsclient_service_load('public_btr');
  $result = $btr->search($params);

  // If the request has the header 'Accept: application/json'
  // or '?format=json' return the output in JSON format and stop.
  bcl::return_json($result['strings']);

  // Set the title.
  drupal_set_title(t('Search Translations'));

  // Add the filter form.
  $output[] = drupal_get_form('btrClient_filter_form', $result['filter']);

  // Output the strings.
  $strings = $result['strings'];
  $pager = $result['pager'];
  if (empty($strings)) {
    drupal_set_message(t('No strings found. Try adjusting the filter options.'));
  }
  else {
    pager_default_initialize($pager['number_of_items'], $pager['items_per_page']);
    $output[] = drupal_get_form('btrClient_search_form', $strings, $lng);

  }

  return $output;
}

/**
 * Form callback for: btrClient_search_form
 */
function btrClient_search_form($form, &$form_state, $strings, $lng) {
  $form = bcl::translateform_build($strings, $lng);
  return $form;
}

/**
 * Submit callback for the form: btrClient_search_form
 */
function btrClient_search_form_submit($form, &$form_state) {
  module_load_include('php', 'btrClient', 'lib/fn/translateform/submit');
  \BTranslator\Client\translateform_submit($form, $form_state);
}
