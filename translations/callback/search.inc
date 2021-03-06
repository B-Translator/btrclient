<?php
/**
 * @file
 * Menu callback for translations/search
 */

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
function btrTranslations_search() {
  // Get the filter parameters from the GET request.
  $params = bcl::filter_get_params();

  // Get the strings that match the search params.
  $btr = oauth2_user_is_authenticated() ?
    wsclient_service_load('btr') :
    wsclient_service_load('public_btr');
  $result = $btr->search($params);

  // If the request has the header 'Accept: application/json'
  // or '?format=json' return the output in JSON format and stop.
  bcl::return_json($result['strings']);

  // Set the title.
  drupal_set_title(t('Search Translations'));

  // Add the filter form.
  $output[] = drupal_get_form('btrTranslations_filter_form', $result['filter']);

  // Output the strings.
  $strings = $result['strings'];
  $pager = $result['pager'];
  if (empty($strings)) {
    drupal_set_message(t('No strings found. Try adjusting the filter options.'));
  }
  else {
    pager_default_initialize($pager['number_of_items'], $pager['items_per_page']);
    $lng = (isset($params['lng']) ? $params['lng'] : bcl::get_translation_lng());
    $output[] = drupal_get_form('btrTranslations_search_form', $strings, $lng);
  }

  return $output;
}

/**
 * Form callback for: btrTranslations_filter_form
 */
function btrTranslations_filter_form($form, &$form_state, $filter) {
  // If the form has no values yet (either it is the first time that
  // the form is opened, or we have a get request), we set the values
  // of the form according to the $filter values.
  if (empty($form_state['values'])) {
    $form_state['values'] = $filter;
  }
  $form = bcl::filter_form($form_state['values']);
  return $form;
}

/**
 * Submit callback for the form: btrTranslations_filter_form
 */
function btrTranslations_filter_form_submit($form, &$form_state) {
  module_load_include('php', 'btrClient', 'lib/fn/filter/submit');
  \BTranslator\Client\filter_submit($form, $form_state);
}

/**
 * Form callback for: btrTranslations_search_form
 */
function btrTranslations_search_form($form, &$form_state, $strings, $lng) {
  $form = bcl::translateform_build($strings, $lng);
  return $form;
}

/**
 * Submit callback for the form: btrTranslations_search_form
 */
function btrTranslations_search_form_submit($form, &$form_state) {
  bcl::translateform_submit($form, $form_state);
}
