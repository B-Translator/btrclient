<?php
/**
 * @file
 * Functions for the block statistics.
 */

/**
 * Called from hook_block_view for topcontrib.
 */
function btrTranslations_statistics_view() {
  // Return cache if possible.
  $cache = cache_get('btrTranslations_block_statistics', 'cache_btrClient');
  if (!empty($cache) && isset($cache->data) && !empty($cache->data)) {
    return $cache->data;
  }

  // Get the statistics.
  try {
    $lng = variable_get('btrClient_translation_lng', 'fr');
    $btr = wsclient_service_load('public_btr');
    $stats = $btr->report_statistics($lng);
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
    $args = array(
      '!nr_votes' => $stat['nr_votes'],
      '!nr_translations' => $stat['nr_translations'],
      '@url_votes' => url('translations/search', array(
                      'query' => array(
                        'lng' => $lng,
                        'limit' => '10',
                        'date_filter' => 'votes',
                        'from_date' => $stat['from_date'],
                      ))),
      '@url_translations' => url('translations/search', array(
                             'query' => array(
                               'lng' => $lng,
                               'limit' => '10',
                               'date_filter' => 'translations',
                               'from_date' => $stat['from_date'],
                             ))),
    );
    $content['stats']['#items'][] = t("Last !period:", array('!period' => $period))
      . '<br/> '
      . t("+ <a href='@url_votes'>!nr_votes votes</a> <br/> + <a href='@url_translations'>!nr_translations translations</a>",
        $args);
  }
  /*
  t('Last week:');
  t('Last month:');
  t('Last year:');
  */

  $block['subject'] = t('Contribution statistics');
  $block['content'] = $content;

  // Cache for 12 hours.
  cache_set('btrTranslations_block_statistics', $block, 'cache_btrClient', time() + 12 * 60 * 60);

  return $block;
}
