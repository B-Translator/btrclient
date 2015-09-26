<?php
/**
 * @file
 * Callback for menu item: 'btr/project/%/%/%/search'
 */

/**
 * Menu callback for 'btr/project/%/%/%/search'
 *
 * Search strings and translations using various filters.
 *
 * If the request header 'Accept: application/json' is set, then
 * the output will be in JSON, otherwise it will be an HTML page.
 */
function btrProject_search($origin, $project, $lng) {
  // Get the filter parameters from the GET request.
  $params = bcl::filter_get_params();
  $params['origin'] = $origin;
  $params['project'] = $project;
  $params['lng'] = $lng;

  // Get the strings that match the search params.
  $btr = oauth2_user_is_authenticated() ?
    wsclient_service_load('btr') :
    wsclient_service_load('public_btr');
  $result = $btr->search($params);

  // If the request has the header 'Accept: application/json'
  // or '?format=json' return the output in JSON format and stop.
  bcl::return_json($result['strings']);

  // Set the title.
  drupal_set_title(t('Search'));

  // Add the filter form.
  $output[] = drupal_get_form('btrProject_filter_form', $result['filter']);

  // Output the strings.
  $strings = $result['strings'];
  if (empty($strings)) {
    drupal_set_message(t('No strings found. Try adjusting the filter options.'));
  }
  else {
    $pager = $result['pager'];
    pager_default_initialize($pager['number_of_items'], $pager['items_per_page']);
    $output[] = drupal_get_form('btrProject_search_form', $strings, $lng);

  }

  return $output;
}

/**
 * Form callback for: btrProject_filter_form
 */
function btrProject_filter_form($form, &$form_state, $filter) {
  // If the form has no values yet (either it is the first time that
  // the form is opened, or we have a get request), we set the values
  // of the form according to the $filter values.
  if (empty($form_state['values'])) {
    $form_state['values'] = $filter;
  }
  $form = bcl::filter_form($form_state['values']);
  unset($form['advanced']['lng']);
  unset($form['advanced']['projects']);
  return $form;
}

/**
 * Submit callback for the form: btrProject_filter_form
 */
function btrProject_filter_form_submit($form, &$form_state) {
  module_load_include('php', 'btrClient', 'lib/fn/filter/submit');
  \BTranslator\Client\filter_submit($form, $form_state);
}

/**
 * Form callback for: btrProject_search_form
 */
function btrProject_search_form($form, &$form_state, $strings, $lng) {
  $form = bcl::translateform_build($strings, $lng);
  return $form;
}

/**
 * Submit callback for the form: btrProject_search_form
 */
function btrProject_search_form_submit($form, &$form_state) {
  bcl::translateform_submit($form, $form_state);
}
