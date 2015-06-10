<?php
/**
 * @file
 * Function: return_json()
 */

namespace BTranslator\Client;

/**
 * If the request has the header 'Accept: application/json'
 * or '?format=json' return the output in JSON format and stop.
 */
function return_json($result) {
  if ( ($_SERVER['HTTP_ACCEPT'] == 'application/json')
    or (isset($_GET['format']) and strtolower($_GET['format']) == 'json') )
    {
      drupal_add_http_header('Content-Type', 'application/json; utf-8');
      print json_encode($result);
      exit;
    }
}
