<?php
/**
 * @file
 * Form: btrVocabulary_form
 */

/**
 * Form callback for: btrVocabulary_form
 */
function btrVocabulary_form($form, &$form_state, $vocabulary, $term) {
  // Include the CSS and JS of the editor.
  bcl::translateform_include_editor();

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

  $form['search'] = [
    '#type' => 'fieldset',

    // submit button
    'submit' => [
      '#value' => '<i class="fa fa-refresh"></i>',
      '#type' => 'submit',
      '#attributes' => [
        'class' => ['btn-primary'],
        'onclick' => 'this.form.target="_self"',
      ],
    ],

    // search term
    'search_term' => [
      '#type' => 'textfield',
      '#autocomplete_path' => $btr_server . '/auto/string/vocabulary/' . $vocabulary,
      '#default_value' => $term,
      '#attributes' => [
        'onblur' => 'this.form.submit()',
      ],
    ],

  ];

  if ($sguid) {
    $form['strings'] = array(
      '#tree' => TRUE,
      '#theme' => 'btrVocabulary_table',
      '#lng' => $lng,
      $sguid => bcl::translateform_string($string, $lng),
    );
  }
  $form['buttons']  = _btrVocabulary_form_buttons($vocabulary, $sguid, $term);
  $form['download-buttons'] = _btrVocabulary_form_download($vocabulary);

  if ($sguid) {
    $form += bcl::translateform_meta($lng, $sguid, $string);
  }

  return $form;
}

/**
 * Main theme function for vocabulary.
 */
function theme_btrVocabulary_table($variables) {
  $element = $variables['element'];

  $rows = array();
  foreach (element_children($element) as $key) {
    $rows[] =  array(
      array(
        'class' => array('translation'),
        'data' => theme('btrClient_translate_translation_list', array('element' => $element[$key])),
      ));
  }

  return theme('table', array(
      'rows' => $rows,
      'attributes' => array(
        'class' => array('l10n-table'),
      )));
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
    $result = $btr->get_random_sguid([
                'target' => 'next',
                'lng' => $lng,
                'scope' => "vocabulary/$vocabulary",
              ]);
    $sguid = $result['sguid'];
    if (!$sguid) {
      return NULL;
    }
  }

  // Get the string details and its translations.
  $btr = wsclient_service_load('public_btr');
  $result = $btr->get_translations($sguid, $lng);
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

  $buttons = bcl::translateform_buttons($lng, $sguid);

  if (!$sguid) {
    $buttons = array();
    $buttons['add_term'] = array(
      '#type' => 'submit',
      '#value' => t('Add Term'),
      '#attributes' => array(
        'onclick' => 'this.form.target="_self"',
      ),
    );
    if (!bcl::user_is_authenticated() and inside_iframe()) {
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
  $attributes = array(
    'class' => array('btn', 'form-submit'),
    'style' => 'margin-left: 10px;',
  );
  if (inside_iframe()) {
    $attributes['target'] = '_blank';
  }
  $buttons['list_all'] = array(
    '#markup' => l('<i class="fa fa-fw fa-list"></i>',
               "btr/project/vocabulary/$vocabulary/$lng/list",
               ['html' => TRUE, 'attributes' => $attributes]),
  );

  // Link to the mobile app.
  if ($vocabulary=='ICT_sq') {
    $url = "http://fjalori.fs.al/#$term";
  }
  else {
    $url = "http://v.btranslator.org/?proj=$vocabulary&lng=$lng#$term";
  }
  $attributes = array(
    'class' => ['btn', 'form-submit'],
    'style' => 'margin-left: 4px;',
  );
  if (!inside_iframe()) {
    $attributes['target'] = '_blank';
  }
  $buttons['mobile'] = array(
    '#markup' => l('<i class="fa fa-fw fa-mobile"></i>', $url,
               array(
                 'html' => TRUE,
                 'attributes' => $attributes,
               )),
  );

  // Display/hide download buttons.
  $buttons['download'] = array(
    '#markup' => '
        <div id="form-item-download">
          <label>
            <input type="checkbox" id="edit-download" name="download">
            <i class="btn fa fa-fw fa-download unchecked"></i>
            <i class="btn fa fa-fw fa-download checked active"></i>
          </label>
        </div>
        ',
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
    '#states' => array(
      'visible' => array(
        ':input[name="download"]' => ['checked' => TRUE],
      )),
    '#attributes' => array(
      'style' => 'float: right; margin: 5px;',
    ),
  );
  foreach ($download_formats as $key => $label) {
    $url = $btr_server . "/translations/vocabulary/export/$vocabulary/$key";
    $download[$key] = array(
      '#markup' => l('<i class="fa fa-fw fa-arrow-down"></i>' . $label,
                 $url, array(
                   'html' => TRUE,
                   'attributes' => [
                     'class' => ['btn'],
                     'style' => 'margin-right: 3px;',
                   ])));
  }
  return $download;
}
