<?php
/**
 * @file
 * Functions for the page: vocabulary
 */

module_load_include('inc', 'btrClient', 'includes/utils');
module_load_include('inc', 'btrClient', 'includes/translation_list/translation_list');
module_load_include('inc', 'btrVocabulary', 'includes/submit');
module_load_include('inc', 'btrVocabulary', 'includes/themes');

/**
 * Menu callback: vocabulary.
 */
function btrVocabulary_page($vocabulary = NULL, $term = NULL) {
  if ($vocabulary == NULL) {
    $lng = bcl::get_translation_lng();
    $vocabulary = 'ICT_' . $lng;
    drupal_goto("vocabulary/$vocabulary");
  }

  // Check whether this is a redirect after login,
  // and if yes call the submit function again.
  bcl::form_resubmit();

  // Set the page title.
  drupal_set_title(t('Vocabulary: !vocabulary',
      array('!vocabulary' => $vocabulary),
      array('context' => 'set the page title')));

  // Add the CSS and JS files.
  drupal_add_css(drupal_get_path('module', 'btrClient') . '/editor/editor.css');
  drupal_add_js(drupal_get_path('module', 'btrClient') . '/editor/jquery.worddiff.js');
  drupal_add_js(drupal_get_path('module', 'btrClient') . '/editor/editor.js');

  // Add RTL style if the current language's direction is RTL.
  $languages = bcl::get_languages();
  list($_, $lng) = explode('_', $vocabulary, 2);
  if ($languages[$lng]['direction'] == LANGUAGE_RTL) {
    drupal_add_css(drupal_get_path('module', 'btrClient') . '/editor-rtl.css');
  }

  // Add the stylesheet of btrVocabulary.
  $css_file = drupal_get_path('module', 'btrVocabulary');
  $css_file .= (bcl::inside_iframe() ?
               '/btrVocabulary.iframe.css' :
               '/btrVocabulary.css');
  drupal_add_css($css_file);

  // Build and return the output.
  $output = drupal_get_form('btrVocabulary_form', $vocabulary, $term);
  return $output;
}

/**
 * Form callback.
 */
function btrVocabulary_form($form, &$form_state, $vocabulary, $term) {
  // Get a string to display on the form.
  $op = $form_state['input']['op'];
  if ($op == t('Save')) {
    $term = $form_state['input']['search_term'];
  }
  $string = _btrVocabulary_get_string($vocabulary, $term);
  $term = ($string ? $string['string'] : $term);
  $sguid = ($string ? $string['sguid'] : NULL);
  list($_, $lng) = explode('_', $vocabulary, 2);
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
    '#autocomplete_path' => $btr_server . '/auto/string/vocabulary/' . $vocabulary,
    '#default_value' => $term,
    '#attributes' => array(
      'onblur' => 'this.form.submit()',
    ),
  );
  $form['go'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
    '#attributes' => array(
      'style' => array('display:none'),
      'onclick' => 'this.form.target="_self"',
    ),
  );

  if ($sguid) {
    $form['strings'] = array(
      '#tree' => TRUE,
      '#theme' => 'btrVocabulary_table',
      '#lng' => $lng,
      $sguid => _btrClient_translate_form_item($form_state, $string, $lng),
    );
  }
  $form['buttons'] = _btrVocabulary_form_buttons($vocabulary, $sguid, $term);

  $form['download'] = _btrVocabulary_form_download($vocabulary);

  // Add metatags (og:title, og:description, og:image).
  // Append the social share buttons and comments to the form.
  if ($sguid and !bcl::inside_iframe()) {
    _btrClient_translate_form_add_metatags($string, $vocabulary);
    _btrClient_translate_form_add_share_and_comment($form, "translations/$lng/$sguid", $term);
  }

  return $form;
}

/**
 * Get and return a string from the vocabulary.
 */
function _btrVocabulary_get_string($vocabulary, $search_term = NULL) {
  list($_, $lng) = explode('_', $vocabulary, 2);

  // Get the sguid of the given search_term, or a random sguid
  // from the vocabulary, if the search_term is empty.
  if (!empty($search_term)) {
    // $sguid = sha1($string . $context)
    $sguid = sha1($search_term . $vocabulary);
  }
  else {
    // Get a random sguid from the vocabulary.
    $btr = bcl::user_is_authenticated() ?
      wsclient_service_load('btr') :
      wsclient_service_load('public_btr');
    $result = $btr->get_random_sguid(
      array(
        'target' => 'next',
        'lng' => $lng,
        'scope' => "vocabulary/$vocabulary",
      ));
    bcl::display_messages($result['messages']);
    $sguid = $result['sguid'];
    if (!$sguid) {
      return NULL;
    }
  }

  // Get the string details and its translations.
  $btr = wsclient_service_load('public_btr');
  $result = $btr->get_translations($sguid, $lng);
  bcl::display_messages($result['messages']);
  $string = $result['string'];
  if (empty($string)) {
    return NULL;
  }

  return $string;
}

/**
 * Add the buttons to the vocabulary form.
 */
function _btrVocabulary_form_buttons($vocabulary, $sguid, $term) {
  list($_, $lng) = explode('_', $vocabulary, 2);
  $btr_server = variable_get('btrClient_server');

  $buttons = array();

  if ($sguid) {
    $attributes = array(
      'class' => 'btn form-submit',
      'style' => 'font-weight:bold',
    );
    if (bcl::inside_iframe()) {
      $attributes['target'] = '_blank';
    }
    $buttons['details'] = array(
      '#markup' => l('#', "translations/$lng/$sguid",
                 array('attributes' => $attributes)),
    );
    $buttons['save'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#attributes' => array(
        'onclick' => 'this.form.target="_self"',
      ),
    );
    // When the user is not authenticated, clicking Save will redirect
    // to login. When we are inside an iFrame, it is better to do the
    // login on a popup window (or a new tab).
    if (!bcl::user_is_authenticated() and bcl::inside_iframe()) {
      $buttons['save']['#attributes']['onclick'] = 'this.form.target="_blank"';
    }
  }
  else {
    $buttons['add_term'] = array(
      '#type' => 'submit',
      '#value' => t('Add Term'),
      '#attributes' => array(
        'onclick' => 'this.form.target="_self"',
      ),
    );
    if (!bcl::user_is_authenticated() and bcl::inside_iframe()) {
      $buttons['add_term']['#attributes']['onclick'] = 'this.form.target="_blank"';
    }
  }

  $buttons['next'] = array(
    '#type' => 'submit',
    '#value' => t('Next'),
    '#attributes' => array(
      'onclick' => 'this.form.target="_self"',
    ),
  );

  // If the user is admin, he can delete the current term.
  if ($sguid and bcl::user_access('btranslator-admin')) {
    $buttons['del_term'] = array(
      '#type' => 'submit',
      '#value' => t('Delete Term'),
      '#attributes' => array(
        'onclick' => 'this.form.target="_self"',
      ),
    );
  }

  // List all items.
  $attributes = array('class' => 'btn form-submit');
  if (bcl::inside_iframe()) {
    $attributes['target'] = '_blank';
  }
  $buttons['list_all'] = array(
    '#markup' => l(t('List'), 'translations/search',
               array(
                 'query' => array(
                   'lng' => $lng,
                   'origin' => 'vocabulary',
                   'project' => $vocabulary,
                 ),
                 'attributes' => $attributes,
               )),
  );

  // Link to the mobile app.
  if ($vocabulary=='ICT_sq') {
    $buttons['mobile'] = array(
      '#markup' => l(t('Go To The Mobile App'), "http://fjalori.fs.al/#$term",
                 array('attributes' => array(
                     'class' => 'btn form-submit',
                   ))),
    );
  }

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
