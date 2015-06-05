<?php
/**
 * @file
 * Functions specific to the menu path: translations/vote .
 */

/**
 * Include utility and theme functions.
 */
require_once dirname(__FILE__) . '/translation_list/translation_list.inc';

/**
 * Menu callback: translations/vote.
 *
 * Vote the best translation for a string, or suggest a better translation.
 *
 * If the request header 'Accept: application/json' is set, then
 * the output will be in JSON, otherwise it will be an HTML page.
 */
function btrClient_vote($lng = NULL, $sguid = NULL) {
  if ($lng == NULL) {
    $lng = bcl::get_translation_lng();
  }
  if ($sguid == NULL) {
    $sguid = 'next';
  }

  if (in_array($sguid, array('next', 'translated', 'untranslated'))) {
    $btr = bcl::user_is_authenticated() ?
      wsclient_service_load('btr') :
      wsclient_service_load('public_btr');
    $result = $btr->get_random_sguid(
      array(
        'target' => $sguid,
        'lng' => $lng,
      ));

    bcl::display_messages($result['messages']);

    $sguid = $result['sguid'];
    if (!$sguid) {
      drupal_set_message(t('No string found!'), 'warning');
      drupal_goto('/');
    }
    else {
      drupal_goto("translations/$lng/$sguid");
    }
    return;
  }
  // last_sguid is used in search, to get similar results.
  $_SESSION['btrClient']['last_sguid'] = $sguid;

  // If there is a $_SESSION['btrClient']['translate_form']['state'],
  // then this is a redirect after login. Call the submit function again.
  if (isset($_SESSION['btrClient']['translate_form']['state'])) {
    $form_state = $_SESSION['btrClient']['translate_form']['state'];
    unset($_SESSION['btrClient']['translate_form']['state']);
    // drupal_form_submit('btrClient_translate_form', $form_state); // ?
    btrClient_translate_form_submit(NULL, $form_state);
  }
  if ($lng == 'all') {
    $lng = 'fr';
    drupal_goto("translations/$lng/$sguid");
    return;
  }

  // Get the string and its translations.
  $btr = wsclient_service_load('public_btr');
  $result = $btr->get_translations($sguid, $lng);

  // If the request has the header 'Accept: application/json'
  // return the output in JSON format and stop.
  if ($_SERVER['HTTP_ACCEPT'] == 'application/json') {
    drupal_add_http_header('Content-Type', 'application/json; utf-8');
    print json_encode($result);
    exit;
  }

  // Display any messages, warnings and errors.
  bcl::display_messages($result['messages']);

  // Go to home if there is no string.
  $string = $result['string'];
  if (empty($string)) {
    drupal_set_message(t('No string found!'), 'warning');
    drupal_goto('/');
    return;
  }

  // Set the title.
  drupal_set_title(t('Vote Translations'));

  // Add the CSS and JS files.
  drupal_add_css(drupal_get_path('module', 'btrClient') . '/editor/editor.css');
  drupal_add_js(drupal_get_path('module', 'btrClient') . '/editor/jquery.worddiff.js');
  drupal_add_js(drupal_get_path('module', 'btrClient') . '/editor/editor.js');

  // Add RTL style if the current language's direction is RTL.
  $languages = bcl::get_languages();
  if ($languages[$lng]['direction'] == LANGUAGE_RTL) {
    drupal_add_css(drupal_get_path('module', 'btrClient') . '/editor-rtl.css');
  }

  // Output the form of the strings and translations.
  $output = drupal_get_form('btrClient_translate_form',
            array($sguid => $string), $lng,
            btrClient_translate_form_buttons($lng));

  return $output;
}
