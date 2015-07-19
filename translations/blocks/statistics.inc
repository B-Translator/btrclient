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

  $block['subject'] = t('Contribution statistics');
  $lng = variable_get('btrClient_translation_lng', 'fr');
  $block['content'] = bcl::render_statistics($lng);

  // Cache for 12 hours.
  cache_set('btrTranslations_block_statistics', $block, 'cache_btrClient', time() + 12 * 60 * 60);

  return $block;
}
