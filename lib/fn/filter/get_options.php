<?php
/**
 * @file
 * Function: filter_get_options()
 */

namespace BTranslator\Client;

/**
 * Return an array of options for the given field and the default value.
 *
 * If being associative ($assoc) is not required then only the keys are
 * returned.
 */
function filter_get_options($field, $assoc = FALSE) {

  switch ($field) {

    case 'limit':
      // Number of results to be displayed per page.
      $options = array(
        '5'  => '5',
        '10' => '10',
        '20' => '20',
        '30' => '30',
        '50' => '50',
      );
      $default = 5;
      break;

    case 'mode':
      // Options for search mode.
      $options = array(
        'natural-strings' => t('Natural search on strings.'),
        'natural-translations' => t('Natural search on translations.'),
        'boolean-strings' => t('Boolean search on strings.'),
        'boolean-translations' => t('Boolean search on translations.'),
      );
      $default = 'natural-strings';
      break;

    case 'date_filter':
      // Which date to filter.
      $options = array(
        'strings' => t('Filter Strings By Date'),
        'translations' => t('Filter Translations By Date'),
        'votes' => t('Filter Votes By Date'),
      );
      $default = 'translations';
      break;

    case 'list_mode':
      // What to list.
      $options = array(
        'all' => '',
        'translated' => t('Translated'),
        'untranslated' => t('Untranslated'),
      );
      $default = 'all';
      break;
  }
  if (!$assoc) {
    $options = array_keys($options);
  }

  return array($options, $default);
}
