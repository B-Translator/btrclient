<?php
/**
 * @file
 * Function: get_languages()
 */

namespace BTranslator\Client;
use \bcl;
use \btr;

/**
 * Return an array of languages and their details.
 */
function get_languages() {
  $cache = cache_get('languages', 'cache_btrClient');
  // Return cache if possible.
  if (!empty($cache) && isset($cache->data) && !empty($cache->data)) {
    return $cache->data;
  }

  // Get an array of languages from the server.
  $btr_server = variable_get('btrClient_server', 'https://dev.btranslator.org');
  $output = drupal_http_request("$btr_server/languages");
  if (isset($output->data)) {
    $languages = json_decode($output->data, TRUE);
  }
  else {
    $languages = array(
      'fr' => array(
        'code' => 'fr',
        'name' => 'French',
        'direction' => LANGUAGE_LTR,
        'plurals' => 2,
      ));
    return $languages;
  }

  // Cache until a general cache wipe.
  cache_set('languages', $languages, 'cache_btrClient', CACHE_TEMPORARY);

  return $languages;
}
