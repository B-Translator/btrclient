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
  if (bcl::user_is_authenticated()) {
    $btr_profile = bcl::user_get_profile();
    $lng = $btr_profile['translation_lng'];
  }
  else {
    $lng = variable_get('btrClient_translation_lng', 'fr');
    if ($lng == 'all')  $lng = 'fr';
  }
  return $lng;
}
