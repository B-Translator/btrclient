<?php
/**
 * @file
 * Function: render_project_stats()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Build the content of the block project_stats.
 */
function render_project_stats($origin, $project, $lng) {
  // Get the project_stats.
  try {
    $btr = wsclient_service_load('public_btr');
    $stats = $btr->report_project_stats($origin, $project, $lng);
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage(), 'error');
    $stats = array();
  }

  $content['project_stats'] = array(
    '#theme' => 'item_list',
    '#items' => array(),
  );
  $items[] = format_plural($stats['strings'], '1 string', '@count strings');
  $items[] = t('!nr translated', ['!nr' => $stats['translated']]);
  $items[] = t('!nr untranslated', ['!nr' => $stats['untranslated']]);
  $items[] = format_plural($stats['translations'], '1 translation', '@count translations');
  $items[] = format_plural($stats['votes'], '1 vote', '@count votes');
  $items[] = format_plural($stats['contributors'], '1 contributor', '@count contributors');
  $items[] = format_plural($stats['subscribers'], '1 subscriber', '@count subscribers');

  $content['project_stats']['#items'] = $items;
  return $content;
}
