<?php
/**
 * @file
 * Functions for the block latest.
 */

/**
 * Called from hook_block_view for latest.
 */
function btrProject_latest_view() {
  // Return cache if possible.
  $cache = cache_get('btrProject_block_latest', 'cache_btrClient');
  if (!empty($cache) && isset($cache->data) && !empty($cache->data)) {
    return $cache->data;
  }

  // Generate the content of the block.
  $content = '';

  $block['subject'] = t('Latest contributions');
  $block['content'] = $content;

  // Cache for 12 hours.
  cache_set('btrProject_block_latest', $block, 'cache_btrClient', time() + 12 * 60 * 60);

  return $block;
}
