<?php
/**
 * @file
 * Functions for the block 'list', which displays a list of the available vocabularies.
 */

/**
 * Called from hook_block_view for list.
 */
function btrVocabulary_list_view() {
  // Build the block.
  $block['subject'] = t('List of Vocabularies');
  $block['content'] = array('#markup' => btrVocabulary_list_view_html());

  return $block;
}

/**
 * Return the html body of the block.
 */
function btrVocabulary_list_view_html() {
  // Get the list of vocabularies.
  $btr_server = variable_get('btrClient_server');
  $result = drupal_http_request($btr_server . '/vocabulary/list');
  if ($result->code != 200) {
    drupal_set_message(t('Could not fetch data from the server.'));
    return '';
  }
  $vocabulary_list = drupal_json_decode($result->data);

  $lng = variable_get('btrClient_translation_lng');
  if ($lng != 'all') {
    // For a single language, just display a plain list of the vocabularies.
    $html =  _btrVocabulary_list_view_html_vocabularies($lng, $vocabulary_list[$lng]);
  }
  else {
    // For all the languages, organize them by language and make it collapsable.
    $html = "<ul class='collapsibleList'>\n";
    $languages = bcl::get_languages();
    foreach ($vocabulary_list as $lng => $vocabularies) {
      $language = $languages[$lng]['name'];
      if (!$language)  $language = $lng;
      $html .= "<li>
        <label for='language-$lng'>$language</label>
        <input type='checkbox' id='language-$lng' />";
      $html .=  _btrVocabulary_list_view_html_vocabularies($lng, $vocabularies);
      $html .= "</li>\n";
    }
  }

  return $html;
}

function _btrVocabulary_list_view_html_vocabularies($lng, $vocabularies) {
  $html = "<ul>\n";
  foreach ($vocabularies as $vocab) {
    $project = $vocab . '_' . $lng;
    $html .= "<li><a href='/vocabulary/$project'>$project</a></li>\n";
  }
  $html .= "</ul>\n";

  return $html;
}
