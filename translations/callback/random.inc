<?php
/**
 * @file
 * Functions specific to the menu path: translations/random
 */

/**
 * Menu callback: translations/random
 *
 * Vote the best translation for a string, or suggest a better translation.
 *
 * If the request header 'Accept: application/json' is set, then
 * the output will be in JSON, otherwise it will be an HTML page.
 */
function btrTranslations_random($lng = NULL, $sguid = NULL) {

  if ($sguid == NULL)  $sguid = 'random';
  if ($lng == NULL or $lng == 'all')  $lng = bcl::get_translation_lng();

  if (in_array($sguid, ['random', 'translated', 'untranslated'])) {
    $btr = bcl::user_is_authenticated() ?
      wsclient_service_load('btr') :
      wsclient_service_load('public_btr');
    $result = $btr->get_random_sguid([
                'target' => $sguid,
                'lng' => $lng,
              ]);
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
    drupal_goto('/');
    return;
  }

  // Output the form of the strings and translations.
  $output = drupal_get_form('btrTranslations_random_form', $sguid, $string, $lng);

  return $output;
}

/**
 * Form callback for: btrTranslations_random_form
 */
function btrTranslations_random_form($form, &$form_state, $sguid, $string, $lng) {
  $form = bcl::translateform_build([$sguid => $string], $lng);
  $form['buttons'] += _btrTranslations_random_form_buttons($lng);
  return $form;
}

/**
 * Get the buttons of the form.
 */
function _btrTranslations_random_form_buttons($lng) {
  $params = array(
    'attributes' => array(
      'class' => array('btn', 'form-submit'),
      'style' => 'margin-right:3px',
    ),
  );
  $buttons['random'] = array(
    '#markup' => l(t('Random'), "translations/$lng/",
               $params),
  );
  $buttons['translated'] = array(
    '#markup' => l(t('Translated'), "translations/$lng/translated",
               $params),
  );
  $buttons['untranslated'] = array(
    '#markup' => l(t('Untranslated'), "translations/$lng/untranslated",
               $params),
  );

  return $buttons;
}

/**
 * Submit callback for the form: btrTranslations_random_form
 */
function btrTranslations_random_form_submit($form, &$form_state) {
  bcl::translateform_submit($form, $form_state);
}
