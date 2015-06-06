<?php
/**
 * @file
 * Function: translateform_build()
 */

namespace BTranslator\Client;
use \bcl;

require_once __DIR__ . '/theme_functions.inc';

/**
 * List strings and the corresponding translations/suggestions.
 *
 * @param array $strings
 *   A multi-dimentional associative array of the string and the corresponding
 *   translations that are to be rendered.
 * @param string $lng
 *   The language code of the translations.
 *
 * @return array
 *   A render array of the given strings and translations.
 */
function translateform_build($strings, $lng) {
  $pager = theme('pager', array('tags' => NULL, 'element' => 0));
  $form = array(
    'langcode' => array(
      '#type' => 'value',
      '#value' => $lng,
    ),
    'pager_top' => array(
      '#weight' => -10,
      '#markup' => $pager,
    ),
    'strings' => array(
      '#tree' => TRUE,
      '#theme' => 'btrClient_translate_table',
      '#lng' => $lng,
    ),

    'buttons' => _buttons($lng),

    'pager_bottom' => array(
      '#weight' => 10,
      '#markup' => $pager,
    ),
  );

  // Fill in string values for the editor.
  foreach ($strings as $string) {
    $sguid = $string['sguid'];
    $form['strings'][$sguid] = bcl::translateform_string($string, $lng);
    // TODO: Display the number of comments for each string.
  }

  // If there is only one string.
  if (count($strings) == 1) {
    // Set the page title (no longer than 20 chars).
    $title = bcl::shorten($string['string'], 20);
    drupal_set_title(t('String: !string', array('!string' => $title),
        array('context' => 'set the page title')));

    // Add metatags (og:title, og:description, og:image).
    include_once __DIR__ . '/_metatags.inc';
    _metatags($string);

    // Append the social share buttons and comments to the form.
    $path = "translations/$lng/$sguid";
    $title = bcl::shorten($string['string'], 50);
    include_once __DIR__ . '/_share_and_comment.inc';
    _share_and_comment($form, $path, $title);
  }

  return $form;
}

/**
 * Get the buttons of the form.
 */
function _buttons($lng) {
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
