<?php
/**
 * @file
 * Function: go_to()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Append to url '?display=iframe' if the page is inside an iframe.
 */
function go_to($url, $options = array()) {
  if (bcl::inside_iframe()) {
    if (!$options['query'])  $options['query'] = array();
    $options['query'] += array('display' => 'iframe');
  }
  drupal_goto($url, $options);
}
