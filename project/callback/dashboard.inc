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
  $output[] = [ '#markup' => '<hr style="clear: both;"/>' ];
  $output[] = _btrProject_dashboard_stats();
  $output[] = [ '#markup' => '<hr style="clear: both;"/>' ];
  $output[] = _btrProject_export_import();

  return $output;
}

/**
 * Render-array for stats.
 */
function _btrProject_dashboard_stats() {
  return [
    'stats' => [
      '#prefix' => '<div class="row">',
      '#suffix' => '</div>',

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
    ],
  ];
}

/**
 * Render-array for export-import.
 */
function _btrProject_export_import() {
  $export = bcl::get_block('btrProject', 'export');
  $export['btrProject_export']['export']['#collapsible'] = TRUE;
  $export['btrProject_export']['export']['#collapsed'] = TRUE;

  $import = bcl::get_block('btrProject', 'import_translations');
  $import['btrProject_import_translations']['import_translations']['#collapsible'] = TRUE;
  $import['btrProject_import_translations']['import_translations']['#collapsed'] = TRUE;

  return [
    'export_import' => [
      '#prefix' => '<div class="row">',
      '#suffix' => '</div>',

      'export' => [
        '#prefix' => '<div class="col-sm-6">',
        $export,
        '#suffix' => '</div>',
      ],

      'import' => [
        '#prefix' => '<div class="col-sm-6">',
        $import,
        '#suffix' => '</div>',
      ],
    ],
  ];
}
