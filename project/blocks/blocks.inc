<?php
/**
 * @file
 * Defining blocks of btrProject.
 */

require_once __DIR__ . '/select.inc';
require_once __DIR__ . '/subscribe.inc';
require_once __DIR__ . '/list.inc';
require_once __DIR__ . '/topcontrib.inc';
require_once __DIR__ . '/statistics.inc';
require_once __DIR__ . '/project_stats.inc';
require_once __DIR__ . '/export.inc';
require_once __DIR__ . '/import.inc';
require_once __DIR__ . '/delete.inc';
require_once __DIR__ . '/import_translations.inc';
require_once __DIR__ . '/latest.inc';
require_once __DIR__ . '/diffs.inc';

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

  // Select a project.
  $blocks['select'] = array(
    'info' => t('B-Translator Project: Select a project'),
    'cache' => DRUPAL_CACHE_CUSTOM,
  );

  // Subscribe to a project.
  $blocks['subscribe'] = array(
    'info' => t('B-Translator Project: Subscribe to a project'),
    'cache' => DRUPAL_NO_CACHE,
  );

  // List of projects.
  $blocks['list'] = array(
    'info' => t('B-Translator Project: List of projects'),
    'cache' => DRUPAL_NO_CACHE,
  );

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

  // Project details and statistics.
  $blocks['project_stats'] = array(
    'info' => t('B-Translator Project: Project details and statistics'),
    'cache' => DRUPAL_CACHE_CUSTOM,
  );

  // Export the project.
  $blocks['export'] = array(
    'info' => t('B-Translator Project: Export'),
    'cache' => DRUPAL_NO_CACHE,
  );

  // Import.
  $blocks['import'] = array(
    'info' => t('B-Translator Project: Import'),
    'cache' => DRUPAL_NO_CACHE,
  );

  // Delete.
  $blocks['delete'] = array(
    'info' => t('B-Translator Project: Delete'),
    'cache' => DRUPAL_NO_CACHE,
  );

  // Import translations.
  $blocks['import_translations'] = array(
    'info' => t('B-Translator Project: Import translations'),
    'cache' => DRUPAL_NO_CACHE,
  );

  // Suggestions and votes submited lately.
  $blocks['latest'] = array(
    'info' => t('B-Translator Project: Latest contributions'),
    'cache' => DRUPAL_CACHE_CUSTOM,
  );

  // Project Diffs.
  $blocks['diffs'] = array(
    'info' => t('B-Translator Project: Project diffs'),
    'cache' => DRUPAL_NO_CACHE,
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
