<?php
/**
 * @file
 * Utility functions for btrVocabulary.
 */

/**
 * Get and return a string from the vocabulary.
 */
function _btrVocabulary_get_string($vocabulary, $search_term = NULL) {
  $lng = btrClient_get_translation_lng();

  // Get the sguid of the given search_term, or a random sguid
  // from the vocabulary, if the search_term is empty.
  if (!empty($search_term)) {
    // $sguid = sha1($string . $context)
    $sguid = sha1($search_term . $vocabulary);
  }
  else {
    // Get a random sguid from the vocabulary.
    $btr = btr_user_is_authenticated() ?
      wsclient_service_load('btr') :
      wsclient_service_load('public_btr');
    $result = $btr->get_random_sguid(
      array(
        'target' => 'next',
        'lng' => $lng,
        'scope' => "vocabulary/$vocabulary",
      ));
    btrClient_display_messages($result['messages']);
    $sguid = $result['sguid'];
    if (!$sguid) {
      return NULL;
    }
  }

  // Get the string details and its translations.
  $btr = wsclient_service_load('public_btr');
  $result = $btr->get_translations($sguid, $lng);
  btrClient_display_messages($result['messages']);
  $string = $result['string'];
  if (empty($string)) {
    return NULL;
  }

  return $string;
}
