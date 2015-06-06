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
  // Check whether this is a redirect after login,
  // and if yes call the submit function again.
  bcl::form_resubmit();

  // Get the search parameters from the GET request.
  $params = btrClient_get_search_params();

  // Get the strings that match the search params.
  $btr = bcl::user_is_authenticated() ?
    wsclient_service_load('btr') :
    wsclient_service_load('public_btr');
  $result = $btr->search($params);
  $filter = $result['filter'];
  $pager = $result['pager'];
  $strings = $result['strings'];

  // If the request has the header 'Accept: application/json'
  // return the output in JSON format and stop.
  if ($_SERVER['HTTP_ACCEPT'] == 'application/json') {
    drupal_add_http_header('Content-Type', 'application/json; utf-8');
    print json_encode($strings);
    exit;
  }

  // Add the CSS and JS files.
  drupal_add_css(drupal_get_path('module', 'btrClient') . '/editor/editor.css');
  drupal_add_js(drupal_get_path('module', 'btrClient') . '/editor/jquery.worddiff.js');
  drupal_add_js(drupal_get_path('module', 'btrClient') . '/editor/editor.js');

  // Add RTL style if the current language's direction is RTL.
  $languages = bcl::get_languages();
  $lng = isset($_GET['lng']) ? $_GET['lng'] : bcl::get_translation_lng();
  if ($languages[$lng]['direction'] == LANGUAGE_RTL) {
    drupal_add_css(drupal_get_path('module', 'btrClient') . '/editor-rtl.css');
  }

  // Set the title.
  drupal_set_title(t('Search Translations'));

  // Add the filter form.
  $output = array(
    drupal_get_form('btrClient_filter_form', $filter),
  );

  // Output the strings.
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
  $form['buttons'] = _btrClient_search_form_buttons($lng);
  $form['langcode'] = array(
    '#type' => 'value',
    '#value' => $lng,
  );
  return $form;
}

/**
 * Get the buttons of the form.
 */
function _btrClient_search_form_buttons($lng) {
  $buttons['login'] = array(
    '#type' => 'submit',
    '#value' => t('Login'),
    '#access' => !bcl::user_is_authenticated(),
  );

  // The submit buttons will appear only when the user has
  // permissions to submit votes and suggestions.
  $translation_lng = variable_get('btrClient_translation_lng', 'all');
  $enable_submit = ($translation_lng == 'all' or ($translation_lng == $lng));
  $buttons['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#access' => $enable_submit,
  );

  return $buttons;
}

/**
 * Submit callback for the form: btrClient_search_form
 */
function btrClient_search_form_submit($form, &$form_state) {
  $op = $form_state['values']['op'];
  if ($op == t('Login')) {
    bcl::user_authenticate($form, $form_state);
  }
  elseif ($op == t('Save')) {
    bcl::user_is_authenticated() ?
      bcl::translateform_save($form_state['values']) :
      bcl::user_authenticate($form, $form_state);
  }
}
