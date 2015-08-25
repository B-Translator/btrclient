<?php
/**
 * @file
 * Functions for the block export.
 */

/**
 * Called from hook_block_view for export.
 */
function btrProject_export_view() {
  // Get the arguments from the path.
  list($origin, $project, $lng) = bcl::get_project_from_path();

  $block['subject'] = t('Export Project');
  $block['content'] = drupal_get_form('btrProject_export_form', $origin, $project, $lng);
  return $block;
}

/**
 * Form callback for: btrProject_export_form
 */
function btrProject_export_form($form, &$form_state, $origin, $project, $lng) {
  $btr_server = variable_get('btrClient_server');

  $form = [
    '#prefix' => '<div class="bcl-ui-filter clear-block">',
    '#suffix' => '</div>',

    'export' => [
      '#type' => 'fieldset',
      '#title' => t('Export Project'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,

      'origin' => [
        '#type' => 'textfield',
        '#title' => t('Origin'),
        '#required' => TRUE,
        '#description' => t('The origin of the project.'),
        '#autocomplete_path' => $btr_server . '/auto/origin',
      ],

      'project' => [
        '#type' => 'textfield',
        '#title' => t('Project'),
        '#required' => TRUE,
        '#description' => t('The name of the project.'),
        '#autocomplete_path' => $btr_server . '/auto/project',
      ],

      'export_mode' => [
        '#type' => 'select',
        '#title' => t('Export Mode'),
        '#description' => t('The export mode that should be used.'),
        '#options' => [
          'original' => t('Export the original translations that were imported.'),
          'most_voted' => t('Export the most voted translations'),
          'preferred_by_me' => t('Prefer translations voted by me'),
          'preferred_by_my_friends' => t('Prefer translations voted by my friends'),
        ],
        '#default_value' => 'most_voted',
      ],

      'submit' => [
        '#value' => t('Export'),
        '#type' => 'submit',
      ],

      'friends' => [
        '#type' => 'textarea',
        '#title' => t('List of Friends'),
        '#description' => t('Enter usernames of your friends, separated by comma.'),
        '#states' => [
          'visible' => [
            ':input[name="export_mode"]' => ['value' => 'preferred_by_my_friends'],
          ],
        ],
      ],
    ],
  ];

  if ($origin)   unset($form['export']['origin']);
  if ($project)  unset($form['export']['project']);

  return $form;
}

/**
 * Submit callback for the form: btrProject_export_form
 */
function btrProject_export_form_submit($form, &$form_state) {
  // If user is not authenticated, redirect to login first.
  if (!bcl::user_is_authenticated()) {
    bcl::user_authenticate($form_state, $redirection = ($form===NULL));
    return;
  }

  // Get the arguments origin/project/lng from the path.
  list($origin, $project, $lng) = bcl::get_project_from_path();

  $values = $form_state['values'];
  if (!$origin)  $origin = check_plain($values['origin']);
  if (!$project)  $project = check_plain($values['project']);
  if (!$lng)  $lng = bcl::get_translation_lng();
  $export_mode = check_plain($values['export_mode']);
  $friends = check_plain($values['friends']);

  if (in_array($export_mode, ['preferred_by_me', 'preferred_by_my_friends'])) {
    $export_mode = 'preferred';
  }

  // Export the project.
  if (bcl::installed_on_server()
    and btr::user_is_project_admin($origin, $project, $lng))
    {
      _btrProject_export_now($origin, $project, $lng, $export_mode, $friends);
    }
  else
    {
      _btrProject_export_schedule($origin, $project, $lng, $export_mode, $friends);
    }
}

/**
 * Export the project immediately.
 */
function _btrProject_export_now($origin, $project, $lng, $export_mode, $friends) {
  $btr_server = variable_get('btrClient_server');
  drupal_goto($btr_server . '/project/export', [
      'absolute' => TRUE,
      'external' => TRUE,
      'query' => [
        'origin' => $origin,
        'project' => $project,
        'lng' => $lng,
        'export_mode' => $export_mode,
        'friends' => $friends,
      ],
    ]);
}

/**
 * Schedule a project export.
 */
function _btrProject_export_schedule($origin, $project, $lng, $export_mode, $friends) {
  $btr = wsclient_service_load('btr');
  $result = $btr->export_projects([
              'origin' => $origin,
              'project' => $project,
              'lng' => $lng,
              'export_mode' => $export_mode,
              'preferred_voters' => $friends,
            ]);
  bcl::display_messages($result['messages']);
}
