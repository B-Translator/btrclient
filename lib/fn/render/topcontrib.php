<?php
/**
 * @file
 * Function: render_topcontrib()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Build the content of top contributors.
 */
function render_topcontrib($period = 'week', $size = '5', $lng = 'fr', $origin = NULL, $project = NULL) {
  // Get the list of top contributers.
  try {
    $btr = wsclient_service_load('public_btr');
    $topusers = $btr->report_topcontrib($period, $size, $lng, $origin, $project);
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage(), 'error');
    $topusers = array();
  }

  $from_date = date('Y-m-d', strtotime("-1 $period"));
  $content = array(
    'first_para' => array(
      '#type' => 'markup',
      '#markup' => t("<p>Top !nr contributors since !date:</p>",
                 array(
                   '!nr' => $size,
                   '!date' => $from_date,
                 )),
    ),
    'second_para' => array(
      '#theme' => 'item_list',
      '#items' => array(),
    ),
  );

  $btr_server = variable_get('btrClient_server');
  foreach ($topusers as $user) {
    $uid = $user['uid'];
    $name = $user['name'];
    $mail = $user['mail'];
    $score = $user['score'];
    $nr_translations = $user['translations'];
    $nr_votes = $user['votes'];
    $url_user = $btr_server . '/user/' . $user['uid'];

    if ($origin == NULL) {
      $url_translations =
        url('translations/search', array(
            'query' => array(
              'lng' => $lng,
              'translated_by' => $user['name'],
              'from_date' => $from_date,
            )));
      $url_votes =
        url('translations/search', array(
            'query' => array(
              'lng' => $lng,
              'voted_by' => $user['name'],
              'date_filter' => 'votes',
              'from_date' => $from_date,
            )));
    }
    else {
      $search_url = "btr/project/$origin/$project/$lng/search";
      $url_translations =
        url($search_url, array(
            'query' => array(
              'translated_by' => $user['name'],
              'from_date' => $from_date,
            )));
      $url_votes =
        url($search_url, array(
            'query' => array(
              'voted_by' => $user['name'],
              'date_filter' => 'votes',
              'from_date' => $from_date,
            )));
    }

    $list_item = "
      <strong><a href='$url_user' target='_blank'>$name</a></strong><br/>
        + <a href='$url_translations' target='_blank'>"
      . format_plural($nr_translations, '1 translation', '@count translations')
      . " </a><br/>
        + <a href='$url_votes' target='_blank'>"
      . format_plural($nr_votes, '1 vote', '@count votes')
      . " </a> ";

    $content['second_para']['#items'][] = $list_item;
  }

  return $content;
}
