<?php
/**
 * @file
 * Callback for menu item: 'btr/project/%/%/%/dashboard'
 */

/**
 * Menu callback for 'btr/project/%/%/%/dashboard'
 */
function btrProject_dashboard($origin, $project, $lng) {
  $output[] = bcl::get_block('btrProject', 'statistics');
  $output[] = bcl::get_block('btrProject', 'topcontrib');
  return $output;
}
