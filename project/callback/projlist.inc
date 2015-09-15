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

      'list_wrapper_1' => [
        '#prefix' => '<div class="col-sm-6">',
        '#suffix' => '</div>',

        'list_wrapper_2' => [
          'listblock_wrapper' => [
            '#prefix' => '<div style="display: inline-block;">',
            bcl::get_block('btrProject', 'list'),
            '#suffix' => '</div>',
          ],

          'refresh_wrapper' => [
            '#prefix' => '<div style="display: inline-block; vertical-align: top; position: fixed; left: 250px;>',
            drupal_get_form('btrProject_projlist_refresh_form'),
            '#suffix' => '</div>',
          ],
        ],
      ],

      'import' => [
        '#prefix' => '<div class="col-sm-6">',
        _btrProject_projlist_import(),
        '#suffix' => '</div>',
      ],
    ],
  ];

  return $output;
}

/**
 * Form callback for: btrProject_projlist_refresh_form
 */
function btrProject_projlist_refresh_form($form, &$form_state) {
  $form = [
    'refresh_icon' => [
      '#value' => '<span class="glyphicon glyphicon-refresh"></span>',
      '#type' => 'submit',
      '#attributes' => ['class' => ['btn-primary']],
    ],
  ];
  return $form;
}

/**
 * Form submit for: btrProject_projlist_refresh_form
 */
function btrProject_projlist_refresh_form_submit($form, &$form_state) {
  // Just clear the block cache, so that the list can be updated.
  cache_clear_all('btrProject_block_list', 'cache_btrClient');
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
