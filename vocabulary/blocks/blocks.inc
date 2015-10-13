<?php
/**
 * @file
 * Defining blocks of btrVocabulary.
 */

require_once __DIR__ . '/list.inc';
require_once __DIR__ . '/import.inc';
require_once __DIR__ . '/delete.inc';

/**
 * Implements hook_block_info().
 *
 * This hook declares what blocks are provided by the module.
 */
function btrVocabulary_block_info() {
  // This hook returns an array, each component of which is an array of block
  // information. The array keys are the 'delta' values used in other block
  // hooks.

  // The required block information is a block description, which is shown
  // to the site administrator in the list of possible blocks. You can also
  // provide initial settings for block weight, status, etc.

  // List of vocabularies.
  $blocks['list'] = array(
    'info' => t('B-Translator Vocabularies: List of vocabularies'),
    'cache' => DRUPAL_NO_CACHE,
  );

  // Import.
  $blocks['import'] = array(
    'info' => t('B-Translator Vocabulary: Import'),
    'cache' => DRUPAL_CACHE_CUSTOM,
  );

  // Delete.
  $blocks['delete'] = array(
    'info' => t('B-Translator Vocabulary: Delete'),
    'cache' => DRUPAL_CACHE_GLOBAL,
  );

  return $blocks;
}

/**
 * Implements hook_block_configure().
 *
 * This hook declares configuration options for blocks provided by this module.
 */
function btrVocabulary_block_configure($delta = '') {
  $form = array();
  // The $delta parameter tells us which block is being configured.
  $config = 'btrVocabulary_' . $delta . '_config';
  if (function_exists($config)) {
    $form = $config();
  }
  return $form;
}


/**
 * Implements hook_block_save().
 *
 * This hook declares how the configured options for a block
 * provided by this module are saved.
 */
function btrVocabulary_block_save($delta = '', $edit = array()) {
  $save = 'btrVocabulary_' . $delta . '_save';
  if (function_exists($save))  $save($edit);
}

/**
 * Implements hook_block_view().
 *
 * This hook generates the contents of the blocks themselves.
 */
function btrVocabulary_block_view($delta = '') {
  // The $delta parameter tells us which block is being requested.
  $view = 'btrVocabulary_' . $delta . '_view';
  if (function_exists($view)) {
    $block = $view();
  }
  return $block;
}
