<?php
/**
 * @file
 * Import a vocabulary.
 */

/**
 * Called from hook_block_view for import.
 */
function btrVocabulary_import_view() {
  $block['subject'] = t('Import Vocabulary');
  $block['content'] = drupal_get_form('btrVocabulary_import_form', $origin, $vocabulary, $lng);
  return $block;
}

/**
 * Form callback for: btrVocabulary_import_form
 */
function btrVocabulary_import_form($form, &$form_state, $origin, $vocabulary, $lng) {
  $description = t('
<p>Create a vocabulary or update an existing one by importing the terms and
translations from a text file. The text file has the same format as the exported
TXT1 file, with each line like this: <code>term &lt;==&gt; translation1 /
translation2 / translation3</code>.</p>'
  );

  // Get an associative array of language codes and names.
  $arr_languages = bcl::get_languages();
  $languages = [];
  foreach ($arr_languages as $lng => $lang) {
    $languages[$lng] = $lang['name'];
  }

  // Build the form.
  $form = [
    'import' => [
      '#type' => 'fieldset',
      '#title' => t('Import vocabulary'),
      '#description' => $description,
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,

      'lng' => [
        '#type' => 'select',
        '#title' => t('Language'),
        '#options' => $languages,
        '#required' => TRUE,
      ],

      'name' => [
        '#type' => 'textfield',
        '#title' => t('Name'),
        '#required' => TRUE,
      ],

      'file' => [
        '#type' => 'file',
        '#title' => t('File of terms and translations'),
      ],

      'submit' => [
        '#value' => t('Import'),
        '#type' => 'submit',
      ],
    ],
  ];
  return $form;
}

/**
 * Implements hook form_validate() for btrVocabulary_import_form.
 */
function btrVocabulary_import_form_validate($form, &$form_state) {
  bcl::uploadfile_validate($extensions='txt', $file_types=['text/plain']);
}


/**
 * Implement hook form_submit() for the form btrVocabulary_import_form
 */
function btrVocabulary_import_form_submit($form, &$form_state) {
  if (!oauth2_user_is_authenticated()) {
    oauth2_user_authenticate($form_state, $redirection = ($form===NULL));
    return;
  }

  $messages = bcl::uploadfile_to_server('vocabulary/import', [
                'lng' => $form_state['values']['lng'],
                'name' => $form_state['values']['name'],
              ]);
  bcl::display_messages($messages);
}
