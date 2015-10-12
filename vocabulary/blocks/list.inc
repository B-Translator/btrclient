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

  $btr_server = variable_get('btrClient_server');
  $result = drupal_http_request($btr_server . '/vocabulary/list');
  if ($result->code != 200) {
    drupal_set_message(t('Could not fetch data from the server.'));
    return '';
  }

  $html = "<ul class='collapsibleList'>\n";
  $vocabulary_list = drupal_json_decode($result->data);
  $languages = bcl::get_languages();
  foreach ($vocabulary_list as $lng => $vocabularies) {
    $language = $languages[$lng]['name'];
    if (!$language)  $language = $lng;
    $html .= "
      <li>
        <label for='language-$lng'>$language</label>
        <input type='checkbox' id='language-$lng' />
        <ul>";
    foreach ($vocabularies as $vocab) {
      $project = $vocab . '_' . $lng;
      $html .= "<li><a href='/vocabulary/$project'>$project</a></li>\n";
    }
    $html .= "
      </ul>
    </li>";
  }

  return $html;
}
