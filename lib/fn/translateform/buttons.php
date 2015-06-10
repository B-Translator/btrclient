<?php
/**
 * @file
 * Function: translateform_buttons()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Get the buttons of the form as a render array.
 */
function translateform_buttons($lng, $sguid = NULL) {
  if ($sguid and bcl::inside_iframe()) {
    $buttons['details'] = [
      '#markup' => l('#', "translations/$lng/$sguid", [
                   'attributes' => [
                     'class' => ['btn'],
                     'style' => 'font-weight:bold; margin-right:3px;',
                     'target' => '_blank',
                   ]]),
    ];
  }
  $buttons['login'] = [
    '#type' => 'submit',
    '#value' => t('Login'),
    '#access' => !bcl::user_is_authenticated(),
    '#attributes' => ['onclick' => 'this.form.target="_self"'],
  ];
  // When we are inside an iFrame, it is better to do the login
  // on a popup window (or a new tab).
  if (bcl::inside_iframe()) {
    $buttons['login']['#attributes']['onclick'] = 'this.form.target="_blank"';
  }

  // The save button will appear only when the user has
  // permissions to submit votes and suggestions.
  $translation_lng = variable_get('btrClient_translation_lng', 'all');
  $enable_save = ($translation_lng == 'all' or ($translation_lng == $lng));
  $buttons['save'] = [
    '#type' => 'submit',
    '#value' => t('Save'),
    '#access' => $enable_save,
    '#attributes' => ['onclick' => 'this.form.target="_self"'],
  ];
  // When the user is not authenticated, clicking Save will redirect
  // to login. When we are inside an iFrame, it is better to do the
  // login on a popup window (or a new tab).
  if (!bcl::user_is_authenticated() and bcl::inside_iframe()) {
    $buttons['save']['#attributes']['onclick'] = 'this.form.target="_blank"';
  }

  return $buttons;
}
