<?php
/**
 * @file
 * Delete a vocabulary.
 */

/**
 * Called from hook_block_view for delete.
 */
function btrVocabulary_delete_view() {
  $block['subject'] = t('Delete Vocabulary');
  $block['content'] = drupal_get_form('btrVocabulary_delete_form');
  return $block;
}

/**
 * Form callback for: btrVocabulary_delete_form
 */
function btrVocabulary_delete_form($form, &$form_state) {
  // Get an associative array of language codes and names.
  $arr_languages = bcl::get_languages();
  $languages = [];
  foreach ($arr_languages as $lng => $lang) {
    $languages[$lng] = $lang['name'];
  }

  // Build the form.
  $form = [
    'delete' => [
      '#type' => 'fieldset',
      '#title' => t('Delete a vocabulary'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,

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

      'submit' => [
        '#value' => t('Delete'),
        '#type' => 'submit',
      ],
    ],
  ];
  return $form;
}

/**
 * Implement hook form_submit() for the form btrVocabulary_delete_form
 */
function btrVocabulary_delete_form_submit($form, &$form_state) {
  if (!oauth2_user_is_authenticated()) {
    oauth2_user_authenticate($form_state, $redirection = ($form===NULL));
    return;
  }

  $btr = wsclient_service_load('btr');
  $result = $btr->delete_vocabulary([
              'lng' => $form_state['values']['lng'],
              'name' => $form_state['values']['name'],
            ]);
  bcl::display_messages([$result['message']]);
}
