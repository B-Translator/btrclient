<?php
/**
 * @file
 * Callback for menu item: 'btr/project/%/%/%/dashboard'
 */

/**
 * Menu callback for 'btr/project/%/%/%/dashboard'
 */
function btrProject_dashboard($origin, $project, $lng) {
  $output = [
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

  return $output;
}
