<?php
/**
 * @file
 * Callback for menu item: 'btr/project/list'
 */

/**
 * Menu callback: btr/project/list
 */
function btrProject_projlist() {
  $output[] = bcl::get_block('btrProject', 'list');
  return $output;
}
