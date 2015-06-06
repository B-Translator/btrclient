<?php
/**
 * @file
 * Callback for menu item: 'btr/project/%/%/%/random'
 */

/**
 * Menu callback for 'btr/project/%/%/%/random'
 *
 * Vote the best translation for a random string from a project, or suggest a
 * better translation.
 *
 * If the request header 'Accept: application/json' is set, then the output will
 * be in JSON, otherwise it will be an HTML page.
 */
function btrProject_random($origin, $project, $lng = NULL, $sguid = NULL) {
  if ($lng == NULL)  $lng = bcl::get_translation_lng();
  if ($sguid == NULL)  $sguid = 'next';

  if (in_array($sguid, array('next', 'translated', 'untranslated'))) {
    $btr = bcl::user_is_authenticated() ?
      wsclient_service_load('btr') :
      wsclient_service_load('public_btr');
    $result = $btr->get_random_sguid(
      array(
        'scope' => "$origin/$project",
        'lng' => $lng,
        'target' => $sguid,
      ));

    bcl::display_messages($result['messages']);

    $sguid = $result['sguid'];
    if (!$sguid) {
      drupal_set_message(t('No string found!'), 'warning');
      drupal_goto('btr/projects/');
    }
    else {
      drupal_goto("btr/project/$origin/$project/$lng/$sguid");
    }
    return;
  }

  // last_sguid is used in search, to get similar results.
  $_SESSION['btrProject']['last_sguid'] = $sguid;

  // Check whether this is a redirect after login,
  // and if yes call the submit function again.
  bcl::form_resubmit();

  if ($lng == 'all') {
    $lng = 'fr';
    drupal_goto("btr/project/$origin/$project/$lng/$sguid");
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
    drupal_goto('btr/projects/');
    return;
  }

  // Set the title.
  drupal_set_title(t('Random String From Project: !project',
      array('!project' => "$origin/$project")));

  // Add the CSS and JS files of the editor.
  drupal_add_css(drupal_get_path('module', 'btrClient') . '/editor/editor.css');
  drupal_add_js(drupal_get_path('module', 'btrClient') . '/editor/jquery.worddiff.js');
  drupal_add_js(drupal_get_path('module', 'btrClient') . '/editor/editor.js');

  // Add RTL style if the current language's direction is RTL.
  $languages = bcl::get_languages();
  if ($languages[$lng]['direction'] == LANGUAGE_RTL) {
    drupal_add_css(drupal_get_path('module', 'btrClient') . '/editor-rtl.css');
  }

  // Add the stylesheet of btrProject.
  $css_file = drupal_get_path('module', 'btrProject');
  $css_file .= (bcl::inside_iframe() ?
               '/style-iframe.css' :
               '/style.css');
  drupal_add_css($css_file);

  // Build and return the output.
  $output = drupal_get_form('btrProject_random_form',
            $origin, $project, $lng, $sguid, $string);

  return $output;
}

/**
 * Form callback for: btrProject_random_form
 */
function btrProject_random_form($form, &$form_state, $origin, $project, $lng, $sguid, $string) {
  $form = bcl::translateform_build([$sguid => $string], $lng);
  $form['buttons'] += _btrProject_random_form_buttons($origin, $project, $lng);
  return $form;
}

/**
 * Get the buttons of the form.
 */
function _btrProject_random_form_buttons($origin, $project, $lng) {
  $attributes = array(
    'attributes' => array(
      'class' => 'btn form-submit',
      'style' => 'margin-right:3px',
    ),
  );
  $buttons['next'] = array(
    '#markup' => l(t('Next'),
               "btr/project/$origin/$project/$lng/",
               $attributes),
  );
  $buttons['translated'] = array(
    '#markup' => l(t('Translated'),
               "btr/project/$origin/$project/$lng/translated",
               $attributes),
  );
  $buttons['untranslated'] = array(
    '#markup' => l(t('Untranslated'),
               "btr/project/$origin/$project/$lng/untranslated",
               $attributes),
  );

  return $buttons;
}

/**
 * Submit callback for the form: btrProject_random_form
 */
function btrProject_random_form_submit($form, &$form_state) {
  module_load_include('php', 'btrClient', 'lib/fn/translateform/submit');
  \BTranslator\Client\translateform_submit($form, $form_state);
}
