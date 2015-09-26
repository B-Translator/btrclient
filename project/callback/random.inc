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
  if ($sguid == NULL)  $sguid = 'random';

  if (in_array($sguid, array('random', 'translated', 'untranslated'))) {
    $btr = oauth2_user_is_authenticated() ?
      wsclient_service_load('btr') :
      wsclient_service_load('public_btr');
    $result = $btr->get_random_sguid([
                'scope' => "$origin/$project",
                'lng' => $lng,
                'target' => $sguid,
              ]);
    $sguid = $result['sguid'];
    if (!$sguid) {
      drupal_set_message(t('No string found!'), 'warning');
      drupal_goto('btr/project/list');
    }
    else {
      drupal_goto("btr/project/$origin/$project/$lng/$sguid");
    }
    return;
  }

  // last_sguid is used in search, to get similar results.
  $_SESSION['btrClient']['last_sguid'] = $sguid;

  if ($lng == 'all') {
    $lng = 'fr';
    drupal_goto("btr/project/$origin/$project/$lng/$sguid");
    return;
  }

  // Get the string and its translations.
  $btr = wsclient_service_load('public_btr');
  $result = $btr->get_translations($sguid, $lng);

  // If the request has the header 'Accept: application/json'
  // or '?format=json' return the output in JSON format and stop.
  bcl::return_json($result);

  // Go to home if there is no string.
  $string = $result['string'];
  if (empty($string)) {
    drupal_set_message(t('No string found!'), 'warning');
    drupal_goto('btr/projects/');
    return;
  }

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
  $params['attributes'] = array(
    'class' => array('btn', 'btn-default', 'form-submit'),
    'style' => 'margin-right:3px',
  );
  $buttons['random'] = array(
    '#markup' => l(t('Random'),
               "btr/project/$origin/$project/$lng/",
               $params),
  );
  $buttons['translated'] = array(
    '#markup' => l(t('Translated'),
               "btr/project/$origin/$project/$lng/translated",
               $params),
  );
  $buttons['untranslated'] = array(
    '#markup' => l(t('Untranslated'),
               "btr/project/$origin/$project/$lng/untranslated",
               $params),
  );

  return $buttons;
}

/**
 * Submit callback for the form: btrProject_random_form
 */
function btrProject_random_form_submit($form, &$form_state) {
  bcl::translateform_submit($form, $form_state);
}
