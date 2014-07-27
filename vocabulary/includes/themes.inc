<?php
/**
 * @file
 * Theme functions for btrVocabulary.
 */

/**
 * Main theme function for vocabulary.
 */
function theme_btrVocabulary_table($variables) {
  $element = $variables['element'];

  $rows = array();
  foreach (element_children($element) as $key) {
    $rows[] = array(
      array(
        'class' => array('source'),
        'data' => theme('btrClient_translate_source', array('element' => $element[$key])),
      ),
      array(
        'class' => array('translation'),
        'data' => theme('btrClient_translate_translation_list', array('element' => $element[$key])),
      ),
    );
  }

  return theme('table', array(
      //'header' => array(t('Term'), t('Translation(s)')),
      'rows' => $rows,
      'attributes' => array(
        'class' => array('l10n-table'),
      )));
}
