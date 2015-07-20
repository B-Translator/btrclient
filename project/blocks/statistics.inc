<?php
/**
 * @file
 * Functions for the block statistics.
 */

/**
 * Called from hook_block_view for topcontrib.
 */
function btrProject_statistics_view() {
  // Return cache if possible.
  $cache = cache_get('btrProject_block_statistics', 'cache_btrClient');
  if (!empty($cache) && isset($cache->data) && !empty($cache->data)) {
    return $cache->data;
  }

  // Get the arguments from the path.
  $args = explode('/', current_path());
  $origin = $args[2];
  $project = $args[3];
  $lng = $args[4];

  // Build the block.
  $block['subject'] = t('Contribution statistics');
  $block['content'] = bcl::render_statistics($lng, $origin, $project);

  // Cache for 12 hours.
  cache_set('btrProject_block_statistics', $block, 'cache_btrClient', time() + 12 * 60 * 60);

  return $block;
}
