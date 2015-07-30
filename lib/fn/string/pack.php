<?php
/**
 * @file
 * Function: string_pack()
 */

namespace BTranslator\Client;

/**
 * Packs a string for storage in the database.
 *
 * @param array $strings
 *   An array of strings.
 *
 * @return string
 *   A packed string with NULL bytes separating each string.
 */
function string_pack($strings) {
  if (is_array($strings)) {
    return implode("\0", $strings);
  }
  else {
    return $strings;
  }
}
