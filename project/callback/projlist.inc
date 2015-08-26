<?php
/**
 * @file
 * Callback for menu item: 'btr/project/list'
 */

/**
 * Menu callback: btr/project/list
 */
function btrProject_projlist() {
  $import = bcl::get_block('btrProject', 'import');
  $import['btrProject_import']['import']['#collapsible'] = TRUE;
  $import['btrProject_import']['import']['#collapsed'] = TRUE;
  $output[] = $import;

  $output[] = bcl::get_block('btrProject', 'list');

  return $output;
}
