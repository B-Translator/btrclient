<?php
/**
 * @file
 * Function: get_translation_lng()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Return the language of translations.
 */
function get_translation_lng() {
  if (oauth2_user_is_authenticated()) {
    $btr_user = oauth2_user_get();
    $lng = $btr_user['translation_lng'];
  }

  if (!$lng) {
    $lng = variable_get('btrClient_translation_lng', 'fr');
  }

  if ($lng == 'all')  $lng = 'fr';

  return $lng;
}
