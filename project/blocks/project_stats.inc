<?php
/**
 * @file
 * Functions for the block project_stats.
 */

/**
 * Called from hook_block_view for topcontrib.
 */
function btrProject_project_stats_view() {
  // Get the arguments from the path.
  $args = explode('/', current_path());
  $origin = $args[2];
  $project = $args[3];
  $lng = $args[4];

  // Return cache if possible.
  $cid = "btrProject_block_project_stats:$origin:$project:$lng";
  $cache = cache_get($cid, 'cache_btrClient');
  if (!empty($cache) && isset($cache->data) && !empty($cache->data)) {
    return $cache->data;
  }

  // Build the block.
  $block['subject'] = t('Project statistics');
  $block['content'] = bcl::render_project_stats($origin, $project, $lng);

  // Cache for 12 hours.
  cache_set($cid, $block, 'cache_btrClient', time() + 12 * 60 * 60);

  return $block;
}
