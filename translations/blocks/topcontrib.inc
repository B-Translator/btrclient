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

  // Get parameters.
  $period = variable_get('btrTranslations_topcontrib_period', 'week');
  $size = variable_get('btrTranslations_topcontrib_size', 5);
  $lng = variable_get('btrClient_translation_lng', 'fr');

  // Build the block view.
  $block['subject'] = t('Top @nr contributors', array('@nr' => $size));
  $block['content'] = bcl::render_topcontrib($period, $size, $lng);

  // Cache for 12 hours.
  cache_set('btrTranslations_block_topcontrib', $block, 'cache_btrClient', time() + 12 * 60 * 60);

  return $block;
}
