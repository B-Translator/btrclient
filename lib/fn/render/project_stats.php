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

  // All strings
  $url_list = "btr/project/$origin/$project/$lng/list";
  $strings = '<a href="' . url($url_list) . '">'
    . format_plural($stats['strings'], '1 string', '@count strings')
    . '</a>';

  // Translated strings.
  $url_list_translated =
    url($url_list, ['query' => ['list_mode' => 'translated']]);
  $percent = ' (' . round($stats['translated'] / $stats['strings'] * 100) . '%) ';
  $translated = '<a href="' . $url_list_translated . '">'
    . t('!nr translated', ['!nr' => $stats['translated']]) . $percent
    . '</a>';

  // Untranslated strings.
  $url_list_untranslated =
    url($url_list, ['query' => ['list_mode' => 'untranslated']]);
  $percent = ' (' . round($stats['untranslated'] / $stats['strings'] * 100) . '%) ';
  $untranslated = '<a href="' . $url_list_untranslated . '">'
    . t('!nr untranslated', ['!nr' => $stats['untranslated']]) . $percent
    . '</a>';

  $item = $strings . '<br/> + ' . $translated . '<br/> + ' . $untranslated;
  $content['project_stats_1'] = array(
    '#theme' => 'item_list',
    '#items' => array($item),
  );

  $content['project_stats_2'] = array(
    '#theme' => 'item_list',
    '#items' => array(
      format_plural($stats['translations'], '1 translation', '@count translations'),
      format_plural($stats['votes'], '1 vote', '@count votes'),
    ),
  );

  $content['project_stats_3'] = array(
    '#theme' => 'item_list',
    '#items' => array(
      format_plural($stats['contributors'], '1 contributor', '@count contributors'),
      format_plural($stats['subscribers'], '1 subscriber', '@count subscribers'),
    ),
  );

  return $content;
}
