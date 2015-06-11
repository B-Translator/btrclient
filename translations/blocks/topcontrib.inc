<?php
/**
 * @file
 * Functions for the block topcontrib.
 */

/**
 * Called from hook_block_config for topcontrib.
 *
 * All we need to provide is the specific configuration options for our
 * block. Drupal will take care of the standard block configuration options
 * (block title, page visibility, etc.) and the save button.
 */
function btrTranslations_topcontrib_config() {
  $form['btrTranslations_topcontrib_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Size of the list'),
    '#description' => t('The number of top contributers that will be shown in the list (5, 10, etc)'),
    '#default_value' => variable_get('btrTranslations_topcontrib_size', 5),
  );
  $form['btrTranslations_topcontrib_period'] = array(
    '#type' => 'select',
    '#title' => t('Accumulation period'),
    '#description' => t('The period during which the contributions will be counted (last week or last month).'),
    '#options' => array(
      'week' => t('Last week'),
      'month' => t('Last month'),
    ),
    '#default_value' => variable_get('btrTranslations_topcontrib_period', 'week'),
  );
  return $form;
}

/**
 * Called from hook_block_save for topcontrib.
 */
function btrTranslations_topcontrib_save($edit) {
  // Have Drupal save them to the database.
  variable_set('btrTranslations_topcontrib_size', $edit['btrTranslations_topcontrib_size']);
  variable_set('btrTranslations_topcontrib_period', $edit['btrTranslations_topcontrib_period']);
}

/**
 * Called from hook_block_view for topcontrib.
 */
function btrTranslations_topcontrib_view() {
  // Return cache if possible.
  $cache = cache_get('btrTranslations_block_topcontrib', 'cache_btrClient');
  if (!empty($cache) && isset($cache->data) && !empty($cache->data)) {
    return $cache->data;
  }

  // The subject is displayed at the top of the block. Note that it
  // should be passed through t() for translation. The title configured
  // for the block using Drupal UI supercedes this one.
  $nr = variable_get('btrTranslations_topcontrib_size', 5);
  $block['subject'] = t('Top @nr contributors', array('@nr' => $nr));

  // Get the list of top contributers.
  try {
    $lng = variable_get('btrClient_translation_lng', 'fr');
    $period = variable_get('btrTranslations_topcontrib_period', 'week');
    $size = variable_get('btrTranslations_topcontrib_size', 5);
    $btr = wsclient_service_load('public_btr');
    $topusers = $btr->report_topcontrib($lng, $period, $size);
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
      '#items' => array(),
      '#theme' => 'item_list',
    ),
  );

  $btr_server = variable_get('btrClient_server');
  foreach ($topusers as $user) {
    $args = array(
      '!uid' => $user['uid'],
      '!name' => $user['name'],
      '!mail' => $user['umail'],
      '!score' => $user['score'],
      '!nr_translations' => $user['translations'],
      '!nr_votes' => $user['votes'],
      '@url_user' => $btr_server . '/user/' . $user['uid'],
      '@url_translations' => url('translations/search', array(
                             'query' => array(
                               'lng' => $lng,
                               'translated_by' => $user['name'],
                               'from_date' => $from_date,
                             ))),
      '@url_votes' => url('translations/search', array(
                      'query' => array(
                        'lng' => $lng,
                        'voted_by' => $user['name'],
                        'date_filter' => 'votes',
                        'from_date' => $from_date,
                      ))),
    );
    $content['second_para']['#items'][] = t("<strong><a href='@url_user' target='_blank'>!name</a></strong><br/><a href='@url_translations' target='_blank'>!nr_translations translations</a> + <a href='@url_votes' target='_blank'>!nr_votes votes</a>",
                                         $args);
  }
  $block['content'] = $content;

  // Cache for 12 hours.
  cache_set('btrTranslations_block_topcontrib', $block, 'cache_btrClient', time() + 12 * 60 * 60);

  return $block;
}
