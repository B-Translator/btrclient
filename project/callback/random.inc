<?php
/**
 * @file
 * Callback for menu item: 'btr/project/%/%/%/random'
 */

/**
 * Include utility and theme functions.
 */
module_load_include('inc', 'btrClient', 'includes/utils');
module_load_include('inc', 'btrClient', 'includes/translation_list/translation_list');

/**
 * Menu callback for 'btr/project/%/%/%/random'
 *
 * It can also be 'btr/project/%/%/%/random/translated'
 * or 'btr/project/%/%/%/random/untranslated'
 * Get a random string from the project and return the
 * review page for it.
 */
function btrProject_random($origin, $project, $lng, $mode = 'next') {
  if (!in_array($mode, ['next', 'translated', 'untranslated'])) {
    $mode = 'next';
  }

  // If there is a $_SESSION['btrProject']['translate_form']['state'],
  // then this is a redirect after login. Call the submit function again.
  if (isset($_SESSION['btrProject']['translate_form']['state'])) {
    $form_state = $_SESSION['btrProject']['translate_form']['state'];
    unset($_SESSION['btrProject']['translate_form']['state']);
    // drupal_form_submit('btrProject_translate_form', $form_state); // ?
    btrProject_translate_form_submit(NULL, $form_state);
  }

  // Get a random sguid from this project.
  $btr = wsclient_service_load('public_btr');
  $result = $btr->get_random_sguid(
    array(
      'target' => $mode,
      'lng' => $lng,
      'scope' => "$origin/$project",
    ));
  btrClient_display_messages($result['messages']);
  $sguid = $result['sguid'];
  if (!$sguid) {
    drupal_set_message(t('No string found!'), 'warning');
    $mode == 'next' ?
      drupal_goto('btr/projects') :
      drupal_goto("btr/project/$origin/$project/$lng");
  }

  // Get the string and its translations.
  $result = $btr->get_translations($sguid, $lng);
  btrClient_display_messages($result['messages']);

  // If the request has the header 'Accept: application/json'
  // return the output in JSON format and stop.
  if ($_SERVER['HTTP_ACCEPT'] == 'application/json') {
    drupal_add_http_header('Content-Type', 'application/json; utf-8');
    print json_encode($result);
    exit;
  }

  // Go to another path if there is no string.
  $string = $result['string'];
  if (empty($string)) {
    drupal_set_message(t('No string found!'), 'warning');
    drupal_goto('btr/projects');
    return;
  }

  // Set the title.
  drupal_set_title(t('Random String From Project: !project',
      array('!project' => "$origin/$project/$lng")));

  // Add the CSS and JS files.
  drupal_add_css(drupal_get_path('module', 'btrClient') . '/editor/editor.css');
  drupal_add_js(drupal_get_path('module', 'btrClient') . '/editor/jquery.worddiff.js');
  drupal_add_js(drupal_get_path('module', 'btrClient') . '/editor/editor.js');

  // Add RTL style if the current language's direction is RTL.
  $languages = btrClient_get_languages();
  if ($languages[$lng]['direction'] == LANGUAGE_RTL) {
    drupal_add_css(drupal_get_path('module', 'btrClient') . '/editor-rtl.css');
  }

  // Output the string and its translations.
  $output = drupal_get_form('btrClient_translate_form', array($sguid => $string), $lng);

  return $output;
}
