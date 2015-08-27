<?php
/**
 * @file
 * Callback for menu item: 'btr/project/%/%/%/dashboard'
 */

/**
 * Menu callback for 'btr/project/%/%/%/dashboard'
 */
function btrProject_dashboard($origin, $project, $lng) {
  $output[] = bcl::get_block('btrProject', 'select');

  $output[] = bcl::get_block('btrProject', 'subscribe');

  $export = bcl::get_block('btrProject', 'export');
  $export['btrProject_export']['export']['#collapsible'] = TRUE;
  $export['btrProject_export']['export']['#collapsed'] = TRUE;
  $output[] = $export;

  $import = bcl::get_block('btrProject', 'import_translations');
  $import['btrProject_import_translations']['import_translations']['#collapsible'] = TRUE;
  $import['btrProject_import_translations']['import_translations']['#collapsed'] = TRUE;
  $output[] = $import;

  $output[] = _btrProject_dashboard_stats();

  return $output;
}

/**
 * Reender array for stats.
 */
function _btrProject_dashboard_stats() {
  return [
    'statistics' => [
      '#prefix' => '<div class="col-sm-4">',
      bcl::get_block('btrProject', 'statistics'),
      '#suffix' => '</div>',
    ],
    'topcontrib' => [
      '#prefix' => '<div class="col-sm-4">',
      bcl::get_block('btrProject', 'topcontrib'),
      '#suffix' => '</div>',
    ],
    'project_stats' => [
      '#prefix' => '<div class="col-sm-4">',
      bcl::get_block('btrProject', 'project_stats'),
      '#suffix' => '</div>',
    ],
  ];
}
