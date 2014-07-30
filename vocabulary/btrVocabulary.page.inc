<?php
/**
 * @file
 * Functions for the page: vocabulary
 */

module_load_include('inc', 'btrClient', 'includes/utils');
module_load_include('inc', 'btrClient', 'includes/translation_list/translation_list');
module_load_include('inc', 'btrVocabulary', 'includes/submit');
module_load_include('inc', 'btrVocabulary', 'includes/themes');
module_load_include('inc', 'btrVocabulary', 'includes/utils');

/**
 * Menu callback: vocabulary.
 */
function btrVocabulary_page($vocabulary = NULL, $term = NULL) {
  $lng = btrClient_get_translation_lng();
  if ($vocabulary == NULL) {
    $vocabulary = 'ICT_' . $lng;
    drupal_goto("vocabulary/$vocabulary");
  }

  // If there is a $_SESSION['btrVocabulary']['form_state'],
  // then this is a redirect after login. Call the submit function again.
  if (isset($_SESSION['btrVocabulary']['form_state'])) {
    $form_state = $_SESSION['btrVocabulary']['form_state'];
    unset($_SESSION['btrVocabulary']['form_state']);
    // drupal_form_submit('btrVocabulary_form', $form_state); // ?
    btrVocabulary_form_submit(NULL, $form_state);
  }

  // Set the page title.
  drupal_set_title(t('Vocabulary: !vocabulary',
      array('!vocabulary' => $vocabulary),
      array('context' => 'set the page title')));

  // Add the CSS and JS files.
  drupal_add_css(drupal_get_path('module', 'btrClient') . '/editor/editor.css');
  drupal_add_js(drupal_get_path('module', 'btrClient') . '/editor/jquery.worddiff.js');
  drupal_add_js(drupal_get_path('module', 'btrClient') . '/editor/editor.js');

  // Add RTL style if the current language's direction is RTL.
  $languages = btrClient_get_languages();
  if ($languages[$lng]['direction'] == LANGUAGE_RTL) {
    drupal_add_css(drupal_get_path('module', 'btrClient') . '/editor-rtl.css');
  }

  // Add the stylesheet of btrVocabulary.
  if (isset($_GET['display']) and $_GET['display'] == 'iframe') {
    drupal_add_css(drupal_get_path('module', 'btrVocabulary') . '/btrVocabulary.iframe.css');
  }
  else {
    drupal_add_css(drupal_get_path('module', 'btrVocabulary') . '/btrVocabulary.css');
  }

  // Build and return the output.
  $output = drupal_get_form('btrVocabulary_form', $vocabulary, $term);
  return $output;
}

/**
 * Form callback.
 */
function btrVocabulary_form($form, &$form_state, $vocabulary, $term) {
  // Get a string to display on the form.
  $form_values = $form_state['values'];
  $term = ($form_values['search_term'] ? $form_values['search_term'] : $term);
  $string = _btrVocabulary_get_string($vocabulary, $term);
  $term = ($string ? $string['string'] : NULL);
  $sguid = ($string ? $string['sguid'] : NULL);
  $lng = btrClient_get_translation_lng();
  $btr_server = variable_get('btrClient_server');

  $form = array(
    'get_query' => array(
      '#type' => 'value',
      '#value' => (isset($form_values['get_query']) ?
                ($form_values['get_query'] + $_GET) : $_GET),
    ),
    'vocabulary' => array(
      '#type' => 'value',
      '#value' => $vocabulary,
    ),
    'langcode' => array(
      '#type' => 'value',
      '#value' => $lng,
    ),
  );

  $form['search_term'] = array(
    '#type' => 'textfield',
    '#autocomplete_path' => $btr_server . '/translations/autocomplete/string/vocabulary/' . $vocabulary,
    '#default_value' => $term,
    '#ajax' => array(
      'callback' => 'btrVocabulary_get_ajax',
      'wrapper' => 'vocabulary-ajax-div',
    ),
  );

  $form['ajax'] = array(
    '#prefix' => '<div id="vocabulary-ajax-div">',
    '#suffix' => '</div>',
  );
  if ($sguid) {
    $form['ajax'] += array(
      'term' => array(
        '#type' => 'value',
        '#value' => $term,
      ),
      'strings' => array(
        '#tree' => TRUE,
        '#theme' => 'btrVocabulary_table',
        '#lng' => $lng,
        $sguid => _btrClient_translate_form_item($form_state, $string, $lng),
      ),
    );
  }
  $form['ajax']['buttons'] = _btrVocabulary_form_buttons($vocabulary, $sguid);

  $form['download'] = _btrVocabulary_form_download($vocabulary);

  // Add metatags (og:title, og:description, og:image).
  _btrClient_translate_form_add_metatags($string);

  return $form;
}

/**
 * Add the buttons to the vocabulary form.
 */
function _btrVocabulary_form_buttons($vocabulary, $sguid) {
  $lng = btrClient_get_translation_lng();
  $btr_server = variable_get('btrClient_server');

  $buttons = array();

  if ($sguid) {
    $buttons['details'] = array(
      '#markup' => l('#', "translations/$lng/$sguid",
                 array('attributes' => array(
                     'class' => 'btn',
                     'style' => 'font-weight:bold',
		     'target' => 'translations',
                   ))),
    );
    $buttons['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
    );
  }
  else {
    $buttons['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Add Term'),
    );
  }

  $buttons['next'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
  );

  // If the user is admin, he can delete the current term.
  if ($sguid and btr_user_access('btranslator-admin')) {
    $buttons['del_term'] = array(
      '#type' => 'submit',
      '#value' => t('Delete Term'),
    );
  }

  // List all items.
  $buttons['list_all'] = array(
    '#markup' => l(t('List'), 'translations/search',
               array(
                 'query' => array(
                   'lng' => $lng,
                   'origin' => 'vocabulary',
                   'project' => $vocabulary,
                 ),
                 'attributes' => array(
                   'class' => 'btn',
		   'target' => 'translations',
                 ),
               )),
  );

  return $buttons;
}

/**
 * Add the download buttons to the vocabulary form.
 */
function _btrVocabulary_form_download($vocabulary) {
  $btr_server = variable_get('btrClient_server');

  // Downloading the whole vocabulary in different formats.
  $download_formats = array(
    'txt1' => 'TXT-1',
    'txt2' => 'TXT-2',
    'org' => 'ORG',
    //'json' => 'JSON',
  );
  $download = array(
    '#type' => 'fieldset',
    '#title' => t('Download'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  foreach ($download_formats as $key => $label) {
    $url = $btr_server . "/translations/vocabulary/export/$vocabulary/$key";
    $download[$key] = array(
      '#markup' => l($label,
                 $url, array('attributes' => array(
                     'class' => 'btn',
                   ))));
  }
  return $download;
}

/**
 * Return the part of the form that is replaced by the ajax call.
 */
function btrVocabulary_get_ajax($form, $form_state) {
  return $form['ajax'];
}
