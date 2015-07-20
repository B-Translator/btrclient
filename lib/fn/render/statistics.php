<?php
/**
 * @file
 * Function: render_statistics()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Build the content of statistics.
 */
function render_statistics($lng = 'fr', $origin = NULL, $project = NULL) {
  // Get the statistics.
  try {
    $btr = wsclient_service_load('public_btr');
    $stats = $btr->report_statistics($lng, $origin, $project);
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage(), 'error');
    $stats = array();
  }

  $content['stats'] = array(
    '#items' => array(),
    '#theme' => 'item_list',
  );
  foreach ($stats as $period => $stat) {
    $nr_votes = $stat['nr_votes'];
    $nr_translations = $stat['nr_translations'];

    if ($origin == NULL) {
      $url_votes =
        url('translations/search', array(
            'query' => array(
              'lng' => $lng,
              'limit' => '10',
              'date_filter' => 'votes',
              'from_date' => $stat['from_date'],
            )));
      $url_translations =
        url('translations/search', array(
            'query' => array(
              'lng' => $lng,
              'limit' => '10',
              'date_filter' => 'translations',
              'from_date' => $stat['from_date'],
            )));
    }
    else {
      $search_url = "btr/project/$origin/$project/$lng/search";
      $url_votes =
        url($search_url, array(
            'query' => array(
              'limit' => '10',
              'date_filter' => 'votes',
              'from_date' => $stat['from_date'],
            )));
      $url_translations =
        url($search_url, array(
            'query' => array(
              'limit' => '10',
              'date_filter' => 'translations',
              'from_date' => $stat['from_date'],
            )));
    }

    $list_item = t("Last !period:", ['!period' => $period])
      . "<br/>
      + <a href='$url_votes' target='_blank'>"
      . format_plural($nr_votes, '1 vote', '@count votes')
      . "</a><br/>
      + <a href='$url_translations' target='_blank'>"
      . format_plural($nr_translations, '1 translation', '@count translations')
      . "</a>";

    $content['stats']['#items'][] = $list_item;
  }
  /*
    t('Last week:');
    t('Last month:');
    t('Last year:');
  */

  return $content;
}
