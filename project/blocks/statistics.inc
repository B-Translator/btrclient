<?php
/**
 * @file
 * Functions for the block statistics.
 */

/**
 * Called from hook_block_view for statistics.
 */
function btrProject_statistics_view() {
  // Get the arguments from the path.
  list($origin, $project, $lng) = bcl::get_project_from_path();

  // Return cache if possible.
  $cid = "btrProject_block_statistics:$lng:$origin:$project";
  $cache = cache_get($cid, 'cache_btrClient');
  if (!empty($cache) && isset($cache->data) && !empty($cache->data)) {
    return $cache->data;
  }

  // Build the block.
  $block['subject'] = t('Contribution statistics');
  $block['content'] = bcl::render_statistics($lng, $origin, $project);

  // Cache for 12 hours.
  cache_set($cid, $block, 'cache_btrClient', time() + 12 * 60 * 60);

  return $block;
}
