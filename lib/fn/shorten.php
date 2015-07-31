<?php
/**
 * @file
 * Function: shorten()
 */

namespace BTranslator\Client;

/**
 * Shorten the given string.
 *
 * From the given (possibly long) string, returns a short string
 * of the given length that can be suitable for title, subject, etc.
 */
function shorten($string, $length) {
  $str = str_replace("\n", ' ', $string);
  $str = str_replace(' <==> ', ' (==) ', $str);
  $str = strip_tags($str);
  if (strlen($str) > $length) {
    $str = substr($str, 0, strrpos(substr($str, 0, $length - 3), ' '));
    $str .= '...';
  }
  $str = str_replace(' (==) ', ' <==> ', $str);
  return $str;
}
