<?php
/**
 * @file
 * Callback for menu item: 'btr/project/list'
 */

/**
 * Menu callback: btr/project/list
 */
function btrProject_projlist() {
  $output = [
    'project_list' => [
      '#prefix' => '<div class="row">',
      '#suffix' => '</div>',

      'list' => [
        '#prefix' => '<div class="col-sm-6">',
        bcl::get_block('btrProject', 'list'),
        '#suffix' => '</div>',
      ],

      'import' => [
        '#prefix' => '<div class="col-sm-6">',
        _btrProject_projlist_import(),
        bcl::get_block('btrProject', 'delete'),
        '#suffix' => '</div>',
      ],
    ],
  ];

  return $output;
}

/**
 * Import a project.
 */
function _btrProject_projlist_import() {
  $import = bcl::get_block('btrProject', 'import');
  $import['btrProject_import']['import']['#collapsible'] = TRUE;
  $import['btrProject_import']['import']['#collapsed'] = TRUE;
  return $import;
}
