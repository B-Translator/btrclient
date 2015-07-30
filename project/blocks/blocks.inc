<?php
/**
 * @file
 * Defining blocks of btrProject.
 */

require_once __DIR__ . '/topcontrib.inc';
require_once __DIR__ . '/statistics.inc';
require_once __DIR__ . '/latest.inc';

/**
 * Implements hook_block_info().
 *
 * This hook declares what blocks are provided by the module.
 */
function btrProject_block_info() {
  // This hook returns an array, each component of which is an array of block
  // information. The array keys are the 'delta' values used in other block
  // hooks.

  // The required block information is a block description, which is shown
  // to the site administrator in the list of possible blocks. You can also
  // provide initial settings for block weight, status, etc.

  // Displays top 5 (or 10) contributors, over the last week (or month).
  $blocks['topcontrib'] = array(
    'info' => t('B-Translator Project: Top contributors during the last period'),
    'cache' => DRUPAL_CACHE_CUSTOM,
  );

  // Suggestion and voting statistics for the last week, month and year.
  $blocks['statistics'] = array(
    'info' => t('B-Translator Project: Contribution statistics (for the last week, month and year)'),
    'cache' => DRUPAL_CACHE_CUSTOM,
  );

  // Suggestions and votes submited lately.
  $blocks['latest'] = array(
    'info' => t('B-Translator Project: Latest contributions'),
    'cache' => DRUPAL_CACHE_CUSTOM,
  );

  return $blocks;
}

/**
 * Implements hook_block_configure().
 *
 * This hook declares configuration options for blocks provided by this module.
 */
function btrProject_block_configure($delta = '') {
  $form = array();
  // The $delta parameter tells us which block is being configured.
  $config = 'btrProject_' . $delta . '_config';
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
function btrProject_block_save($delta = '', $edit = array()) {
  $save = 'btrProject_' . $delta . '_save';
  if (function_exists($save))  $save($edit);
}

/**
 * Implements hook_block_view().
 *
 * This hook generates the contents of the blocks themselves.
 */
function btrProject_block_view($delta = '') {
  // The $delta parameter tells us which block is being requested.
  $view = 'btrProject_' . $delta . '_view';
  if (function_exists($view)) {
    $block = $view();
  }
  return $block;
}
