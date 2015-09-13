<?php
/**
 * @file
 * Function uploadfile_to_server().
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Make an http request for uploading a file to the B-Translator server.
 *
 * This is done with curl because wsclient cannot handle it.
 *
 * TODO: Replace this function and wsclient by Guzzle.
 */
function uploadfile_to_server($endpoint, $params = array()) {
  $btr = wsclient_service_load('btr');

  // Get an access_token.
  module_load_include('inc', 'oauth2_client', 'oauth2_client');
  $oauth2_settings = $btr->settings['authentication']['oauth2'];
  $oauth2_client = new \OAuth2\Client($oauth2_settings);
  try {
    $access_token = $oauth2_client->getAccessToken();
  }
  catch (\Exception $e) {
    return [[$e->getMessage(), 'error']];
  }

  // Get the details of the uploaded file.
  $file_name = $_FILES['files']['name']['file'];
  $file_tmp_name = $_FILES['files']['tmp_name']['file'];
  $file_size = $_FILES['files']['size']['file'];

  // Make an http request with curl.
  $ch = curl_init($btr->url . $endpoint);
  @curl_setopt_array($ch, array(
      CURLOPT_POST => TRUE,
      CURLOPT_POSTFIELDS => array(
        'file' => "@$file_tmp_name;filename=$file_name",
      ) + $params,
      CURLOPT_HTTPHEADER => array(
        'Content-Type: multipart/form-data',
        'Authorization: Bearer ' . $access_token,
        'Accept: application/json',
      ),
      CURLOPT_RETURNTRANSFER => TRUE,
    ) +
    $btr->settings['curl options']
  );
  $result = curl_exec($ch);

  // Check for any errors and get the result.
  if (curl_errno($ch)) {
    $messages = [[curl_error($ch), 'error']];
  }
  else {
    $result = json_decode($result, TRUE);
    if (isset($result['messages'])) {
      $messages = $result['messages'];
    }
    else {
      $messages = [[serialize($result), 'error']];
    }
  }
  curl_close($ch);

  return $messages;
}
