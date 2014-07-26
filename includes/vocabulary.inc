<?php
/**
 * @file
 * Functions specific to the menu path: vocabulary .
 */

/**
 * Include utility and theme functions.
 */
require_once(dirname(__FILE__) . '/translation_list/translation_list.inc');
require_once(dirname(__FILE__) . '/utils.inc');

/**
 * Menu callback: vocabulary.
 */
function btrClient_vocabulary($vocabulary = NULL, $term = NULL) {
  $lng = btrClient_get_translation_lng();
  if ($vocabulary == NULL) {
    $vocabulary = 'ICT_' . $lng;
    drupal_goto("vocabulary/$vocabulary");
  }

  // If there is a $_SESSION['btrClient']['vocabulary_form']['state'],
  // then this is a redirect after login. Call the submit function again.
  if (isset($_SESSION['btrClient']['vocabulary_form']['state'])) {
    $form_state = $_SESSION['btrClient']['vocabulary_form']['state'];
    unset($_SESSION['btrClient']['vocabulary_form']['state']);
    // drupal_form_submit('btrClient_vocabulary_form', $form_state); // ?
    btrClient_vocabulary_form_submit(NULL, $form_state);
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

  // Build and return the output.
  $output = drupal_get_form('btrClient_vocabulary_form', $vocabulary, $term);
  return $output;
}

/**
 * Form callback.
 */
function btrClient_vocabulary_form($form, &$form_state, $vocabulary, $term) {
  // Get a string to display on the form.
  $form_state_term = $form_state['values']['search_term'];
  $search_term = ($form_state_term ? $form_state_term : $term);
  $string = _btrClient_vocabulary_getstring($vocabulary, $search_term);
  if (!$string) {
    drupal_set_message(t('No string found!'), 'error');
    return '';
  }

  $term = $string['string'];
  $sguid = $string['sguid'];
  $lng = btrClient_get_translation_lng();
  $btr_server = variable_get('btrClient_server');

  $form = array(
    'langcode' => array(
      '#type' => 'value',
      '#value' => $lng,
    ),
    'vocabulary' => array(
      '#type' => 'value',
      '#value' => $vocabulary,
    ),
    'term' => array(
      '#type' => 'value',
      '#value' => $term,
    ),
  );

  $form['search_term'] = array(
    '#type' => 'textfield',
    '#autocomplete_path' => $btr_server . '/translations/autocomplete/string/vocabulary/' . $vocabulary,
    //'#default_value' => $search_term,
    '#ajax' => array(
      'callback' => 'btrClient_vocabulary_terms_callback',
      'wrapper' => 'vocabulary-terms-div',
      'keypress' => TRUE,
      'effect' => 'fade',
    ),
  );

  $form['strings'] = array(
    '#tree' => TRUE,
    '#prefix' => '<div id="vocabulary-terms-div">',
    '#suffix' => '</div>',
    '#theme' => 'btrClient_vocabulary_table',
    '#lng' => $lng,
    $sguid => _btrClient_translate_form_item($form_state, $string, $lng),
  );

  $form += _btrClient_vocabulary_form_buttons($vocabulary, $sguid);

  return $form;
}

/**
 * Add the buttons to the vocabulary form.
 */
function _btrClient_vocabulary_form_buttons($vocabulary, $sguid) {
  $lng = btrClient_get_translation_lng();
  $btr_server = variable_get('btrClient_server');

  // Add the first row of buttons (#, Save, Next, List All Terms).
  $form['buttons'] = array(
    '#prefix' => '<div id="buttons" style="margin-bottom: 5px">',
    '#suffix' => '</div>',
    'details' => array(
      '#markup' => l('#', "translations/$lng/$sguid",
                 array('attributes' => array(
                     'class' => 'btn',
                     'style' => 'font-weight:bold; margin-right:4px',
                   ))),
    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save'),
    ),
    'next' => array(
      '#type' => 'submit',
      '#value' => t('Next'),
    ),
    'list_all' => array(
      '#markup' => l(t('List All Terms'), "translations/search",
                 array(
                   'query' => array(
                     'lng' => $lng,
                     'origin' => 'vocabulary',
                     'project' => $vocabulary,
                   ),
                   'attributes' => array(
                     'class' => 'btn',
                     'style' => 'margin-right:4px',
                   ),
                 )),
    ),
  );

  // Buttons for adding/deleting a term.
  /*
  $form['admin'] = array(
    '#prefix' => '<div id="admin" style="margin-bottom: 5px">',
    '#suffix' => '</div>',
    'add' => array(
      '#markup' => l(t('Add Term'), "vocabulary/$vocabulary/add_term",
                 array('attributes' => array(
                     'class' => 'btn',
                     'style' => 'margin-right:4px',
                   ))),
    ),
    'del' => array(
      '#markup' => l(t('Delete Term'), "vocabulary/$vocabulary/del_term",
                 array('attributes' => array(
                     'class' => 'btn',
                     'style' => 'margin-right:4px',
                   ))),
    ),
  );
  */

  // Downloading the whole vocabulary in different formats.
  $download_formats = array(
    'pdf' => 'PDF',
    'txt1' => 'TXT-1',
    'txt2' => 'TXT-2',
    'org' => 'ORG',
  );
  $form['download'] = array(
    '#type' => 'fieldset',
    '#title' => t('Download'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  foreach ($download_formats as $key => $label) {
    $url = $btr_server . "/translations/vocabulary/export/$vocabulary/$key";
    $form['download'][$key] = array(
      '#markup' => l($label,
                 $url, array('attributes' => array(
                     'class' => 'btn',
                     'style' => 'margin-right:4px',
                   ))));
  }
  return $form;
}

/**
 * Return the part of the form that is replaced by the ajax call.
 */
function btrClient_vocabulary_terms_callback($form, $form_state) {
  return $form['strings'];
}

/**
 * Main theme function for vocabulary.
 */
function theme_btrClient_vocabulary_table($variables) {
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

/**
 * Form submit callback for btrClient_vocabulary_form().
 */
function btrClient_vocabulary_form_submit($form, &$form_state) {
  $form_values = $form_state['values'];

  // Check that the button clicked was either Save, or Next, etc.
  $op = $form_values['op'];
  if (!in_array($op, array(t('Save'), t('Next')))) {
    return;
  }

  // If user is not authenticated, then login first (redirect to login).
  if ($op == t('Save') and !btr_user_is_authenticated()) {
    if (_btrClient_installed_on_server()) {
      drupal_goto('user/login', array('query' => drupal_get_destination()));
    }
    else {
      $_SESSION['btrClient']['vocabulary_form']['state'] = $form_state;
      oauth2_login();
    }
  }

  // Save the values selected on the form (vote or new suggestion).
  if ($op == t('Save') and btr_user_is_authenticated()) {
    _btrClient_translate_form_save($form_values);

    // Redirect to the same term.
    $term = $form_values['term'];
    $vocabulary = $form_values['vocabulary'];
    drupal_goto("vocabulary/$vocabulary/$term");
  }

  if ($op == t('Next')) {
    $vocabulary = $form_values['vocabulary'];
    drupal_goto("vocabulary/$vocabulary");
  }
}

/**
 * Get and return a string from the vocabulary.
 */
function _btrClient_vocabulary_getstring($vocabulary, $search_term = NULL) {
  $lng = btrClient_get_translation_lng();

  // Get the sguid of the given search_term, or a random sguid
  // from the vocabulary, if the search_term is empty.
  if (!empty($search_term)) {
    // $sguid = sha1($string . $context)
    $sguid = sha1($search_term . 'vocabulary');
  }
  else {
    // Get a random sguid from the vocabulary.
    $btr = btr_user_is_authenticated() ?
      wsclient_service_load('btr') :
      wsclient_service_load('public_btr');
    $result = $btr->get_random_sguid(
      array(
        'target' => 'next',
        'lng' => $lng,
        'scope' => "vocabulary/$vocabulary",
      ));
    btrClient_display_messages($result['messages']);
    $sguid = $result['sguid'];
    if (!$sguid) {
      return NULL;
    }
  }

  // Get the string details and its translations.
  $btr = wsclient_service_load('public_btr');
  $result = $btr->get_translations($sguid, $lng);
  btrClient_display_messages($result['messages']);
  $string = $result['string'];
  if (empty($string)) {
    return NULL;
  }

  return $string;
}
