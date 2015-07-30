<?php
/**
 * @file
 * Function: get_block()
 */

namespace BTranslator\Client;


/**
 * Get and return a block as a renderable array.
 */
function get_block($module, $delta) {
  $block = array(block_load($module, $delta));
  $renderable_array = _block_get_renderable_array(_block_render_blocks($block));
  return $renderable_array;
}
