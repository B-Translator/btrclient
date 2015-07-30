<?php
/**
 * @file
 * Functions for the block statistics.
 */

/**
 * Called from hook_block_view for topcontrib.
 */
function btrTranslations_statistics_view() {
  $lng = variable_get('btrClient_translation_lng', 'fr');

  // Return cache if possible.
  $cid = "btrTranslations_block_statistics:$lng";
  $cache = cache_get($cid, 'cache_btrClient');
  if (!empty($cache) && isset($cache->data) && !empty($cache->data)) {
    return $cache->data;
  }

  $block['subject'] = t('Contribution statistics');
  $block['content'] = bcl::render_statistics($lng);

  // Cache for 12 hours.
  cache_set($cid, $block, 'cache_btrClient', time() + 12 * 60 * 60);

  return $block;
}
