<?php
/**
 * @file
 * Return a random string in a suitable form for the twitter.
 */

/**
 * Callback for 'btr/tweet'.
 */
function btrClient_tweet($origin = NULL, $project = NULL) {
  // Get the translation language.
  $lng = variable_get('btrClient_translation_lng', 'fr');

  // Get a tweet from the B-Translator server.
  $btr_server = variable_get('btrClient_server', 'http://dev.btranslator.org');
  $url = $btr_server . "/tweet/$lng/$origin/$project";
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
    if ($project == 'ICT_sq') {
      $url = "http://fjalori.fs.al/#$term";
    }
    else {
      $url = url("vocabulary/$project/$term", array('absolute' => TRUE));
    }
  }
  else {
    $url = url("translations/$lng/$sguid", array('absolute' => TRUE));
  }
  $twitter .= ' ' . $url;

  print $twitter;
  exit(0);
}
