<?php
/**
 * @file
 * Return a random string in a suitable form for the twitter.
 */

/**
 * Callback for translations/twitter.
 */
function btrCore_twitter($origin = NULL, $project = NULL) {
  // Get the translation language.
  $lng = variable_get('btrClient_translation_lng', 'fr');

  // Get a tweet from the B-Translator server.
  $btr_server = variable_get('btrClient_server', 'https://dev.btranslator.org');
  $url = $btr_server . "/translations/twitter/$lng/$origin/$project";
  $output = drupal_http_request($url, array(
              'headers' => array(
                'Accept' => 'application/json',
              )));
  $result = json_decode($output->data, TRUE);
  $sguid = $result['sguid'];
  $twitter = $result['twitter'];

  // Append hash tags.
  $twitter .= " #l10n_$lng";
  if (!empty($origin)) {
    $twitter .= " #$origin";
  }
  if (!empty($project)) {
    $twitter .= " #$project";
  }

  // Append the url.
  if ((empty($origin) and empty($project)) or ($origin == 'vocabulary')) {
    if (empty($project)) {
      $project = "ICT_$lng";
    }
    list($term, $x) = explode(' <==> ', $twitter, 2);
    $url = url("vocabulary/$project/$term", array('absolute' => TRUE));
  }
  else {
    $url = url("translations/$lng/$sguid", array('absolute' => TRUE));
  }
  $twitter .= ' ' . $url;

  print $twitter;
  exit(0);
}