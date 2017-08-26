<?php
/**
 * @file
 * Function: http_request()
 */

namespace BTranslator\Client;

/**
 * Performs an HTTP request.
 *
 * Improves drupal_http_request() by allowing to ignore ssl certificates
 * (can be useful for development and testing).
 */
function http_request($url, array $options = array()) {
  $skipssl = variable_get('oauth2_login_skipssl', TRUE);
  if ($skipssl) {
    $options['context']
      = stream_context_create(array(
          'ssl' => array(
            'verify_peer' => FALSE,
            'verify_peer_name' => FALSE,
          ),
        ));
  }
  return drupal_http_request($url, $options);
}
