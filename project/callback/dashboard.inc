<?php
/**
 * @file
 * Callback for menu item: 'btr/project/%/%/%/dashboard'
 */

/**
 * Menu callback for 'btr/project/%/%/%/dashboard'
 */
function btrProject_dashboard($origin, $project, $lng) {
  $output[] = _btrProject_get_block('btrProject', 'statistics');
  $output[] = _btrProject_get_block('btrProject', 'topcontrib');
  return $output;
}

/**
 * Get and return a block as a renderable array.
 */
function _btrProject_get_block($module, $delta) {
  $block = array(block_load($module, $delta));
  $renderable_array = _block_get_renderable_array(_block_render_blocks($block));
  return $renderable_array;
}
