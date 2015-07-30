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
function btrProject_topcontrib_config() {
  $form['btrProject_topcontrib_size'] = array(
    '#type' => 'textfield',
    '#title' => t('Size of the list'),
    '#description' => t('The number of top contributers that will be shown in the list (5, 10, etc)'),
    '#default_value' => variable_get('btrProject_topcontrib_size', 5),
  );
  $form['btrProject_topcontrib_period'] = array(
    '#type' => 'select',
    '#title' => t('Accumulation period'),
    '#description' => t('The period during which the contributions will be counted (last week or last month).'),
    '#options' => array(
      'week' => t('Last week'),
      'month' => t('Last month'),
    ),
    '#default_value' => variable_get('btrProject_topcontrib_period', 'week'),
  );
  return $form;
}

/**
 * Called from hook_block_save for topcontrib.
 */
function btrProject_topcontrib_save($edit) {
  // Have Drupal save them to the database.
  variable_set('btrProject_topcontrib_size', $edit['btrProject_topcontrib_size']);
  variable_set('btrProject_topcontrib_period', $edit['btrProject_topcontrib_period']);
}

/**
 * Called from hook_block_view for topcontrib.
 */
function btrProject_topcontrib_view() {
  // Get parameters.
  $period = variable_get('btrProject_topcontrib_period', 'week');
  $size = variable_get('btrProject_topcontrib_size', 5);
  $args = explode('/', current_path());
  $origin = $args[2];
  $project = $args[3];
  $lng = $args[4];

  // Return cache if possible.
  $cid = "btrProject_block_topcontrib:$period:$size:$lng:$origin:$project";
  $cache = cache_get($cid, 'cache_btrClient');
  if (!empty($cache) && isset($cache->data) && !empty($cache->data)) {
    return $cache->data;
  }

  // Build the block view.
  $block['subject'] = t('Top @nr contributors', array('@nr' => $size));
  $block['content'] = bcl::render_topcontrib($period, $size, $lng, $origin, $project);

  // Cache for 12 hours.
  cache_set($cid, $block, 'cache_btrClient', time() + 12 * 60 * 60);

  return $block;
}
