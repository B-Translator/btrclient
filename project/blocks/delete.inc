<?php
/**
 * @file
 * Delete a project.
 */

/**
 * Called from hook_block_view for delete.
 */
function btrProject_delete_view() {
  $block['subject'] = t('Delete Project');
  $block['content'] = drupal_get_form('btrProject_delete_form');
  return $block;
}

/**
 * Form callback for: btrProject_delete_form
 */
function btrProject_delete_form($form, &$form_state) {
  // Suggest the username as the default value for the origin.
  $btr_user = bcl::btr_user_get();
  $default_origin = str_replace(' ', '', $btr_user['name']);
  if (!$default_origin) $default_origin = 'test';

  // Build the form.
  $form = [
    'delete' => [
      '#type' => 'fieldset',
      '#title' => t('Delete a project'),
      //'#description' => $description,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,

      'origin' => [
        '#type' => 'textfield',
        '#title' => t('Origin'),
        '#default_value' => $default_origin,
        '#required' => TRUE,
      ],

      'project' => [
        '#type' => 'textfield',
        '#title' => t('Project'),
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
 * Implement hook form_submit() for the form btrProject_delete_form
 */
function btrProject_delete_form_submit($form, &$form_state) {
  if (!bcl::user_is_authenticated()) {
    bcl::user_authenticate($form_state, $redirection = ($form===NULL));
    return;
  }

  $btr = wsclient_service_load('btr');
  $result = $btr->delete_project([
              'origin' => $form_state['values']['origin'],
              'project' => $form_state['values']['project'],
            ]);
  bcl::display_messages([$result['message']]);
}
