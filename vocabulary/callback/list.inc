<?php
/**
 * @file
 * Callback for menu item: 'vocabulary/list'
 */

/**
 * Menu callback: vocabulary/list
 */
function btrVocabulary_list() {
  $output = [
    'vocabulary_list' => [
      '#prefix' => '<div class="row">',
      '#suffix' => '</div>',

      'list' => [
        '#prefix' => '<div class="col-sm-6">',
        bcl::get_block('btrVocabulary', 'list'),
        '#suffix' => '</div>',
      ],

      /*
      'import' => [
        '#prefix' => '<div class="col-sm-6">',
        _btrVocabulary_list_import(),
        bcl::get_block('btrVocabulary', 'delete'),
        '#suffix' => '</div>',
      ],
      */
    ],
  ];

  return $output;
}

/**
 * Import a vocabulary.
 */
function _btrVocabulary_list_import() {
  $import = bcl::get_block('btrVocabulary', 'import');
  $import['btrVocabulary_import']['import']['#collapsible'] = TRUE;
  $import['btrVocabulary_import']['import']['#collapsed'] = TRUE;
  return $import;
}
