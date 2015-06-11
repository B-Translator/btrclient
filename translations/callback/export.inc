<?php
/**
 * @file
 * Export the best translations of a project into PO format.
 */

/**
 * Callback for translations/export.
 *
 * Export the best translations of a project into PO format.
 */
function btrTranslations_export() {
  return drupal_get_form('btrTranslations_export_form');
}


/**
 * Export form.
 */
function btrTranslations_export_form() {
  $form = array(
    '#prefix' => '<div class="bcl-ui-filter clear-block">',
    '#suffix' => '</div>',
  );

  $form['export'] = array(
    '#type' => 'fieldset',
    '#title' => t('Export a Project'),
    '#description' => t('<p>Submit a request for exporting a project. When the request is completed you will be notified by email.</p><br/>'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  $btr_server = variable_get('btrClient_server');
  $form['export']['origin'] = array(
    '#type' => 'textfield',
    '#title' => t('Origin'),
    '#required' => TRUE,
    '#description' => t('The origin of the project.'),
    '#autocomplete_path' => $btr_server . '/auto/origin',
  );

  $form['export']['project'] = array(
    '#type' => 'textfield',
    '#title' => t('Project'),
    '#required' => TRUE,
    '#description' => t('The name of the project.'),
    '#autocomplete_path' => $btr_server . '/auto/project',
  );

  $form['export']['export_mode'] = array(
    '#type' => 'select',
    '#title' => t('Export Mode'),
    '#description' => t('The export mode that should be used.'),
    '#options' => array(
      'original' => t('Export the original translations that were imported.'),
      'most_voted' => t('Export the most voted translations'),
      'preferred_by_me' => t('Prefer translations voted by me'),
      'preferred_by_my_friends' => t('Prefer translations voted by my friends'),
    ),
    '#default_value' => 'most_voted',
  );

  $form['export']['friends'] = array(
    '#type' => 'textarea',
    '#title' => t('List of Friends'),
    '#description' => t('Enter usernames of your friends, separated by comma.'),
    '#states' => array(
      'visible' => array(
        ':input[name="export_mode"]' => array('value' => 'preferred_by_my_friends'),
      ),
    ),
  );

  $form['buttons'] = array(
    // '#prefix' => '<div class="export-submit">',
    // '#suffix' => '</div>',
  );
  $form['buttons']['submit'] = array(
    '#value' => t('Export'),
    '#type' => 'submit',
  );

  return $form;
}


/**
 * Submission handler for export form.
 *
 * Send the export request to the B-Translator server.
 */
function btrTranslations_export_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  $values += array(
    'origin' => NULL,
    'project' => NULL,
    'export_mode' => NULL,
    'friends' => NULL,
  );

  $origin = check_plain($values['origin']);
  $project = check_plain($values['project']);
  $export_mode = check_plain($values['export_mode']);
  $friends = check_plain($values['friends']);

  if (in_array($export_mode, array('preferred_by_me', 'preferred_by_my_friends'))) {
    $export_mode = 'preferred';
  }

  $btr = wsclient_service_load('btr');
  $result = $btr->export_projects(array(
              'origin' => $origin,
              'project' => $project,
              'lng' => bcl::get_translation_lng(),
              'export_mode' => $export_mode,
              'preferred_voters' => $friends,
            ));
  bcl::display_messages($result['messages']);
}
