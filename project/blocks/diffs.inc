<?php
/**
 * @file
 * Project Diffs.
 */

/**
 * Called from hook_block_view for diffs.
 */
function btrProject_diffs_view() {
  // Get the arguments from the path.
  list($origin, $project, $lng) = bcl::get_project_from_path();

  $block['subject'] = t('Project Diffs');
  $block['content'] = btrProject_diffs_content($origin, $project, $lng);
  return $block;
}

/**
 * Render array for the content of the block diffs.
 */
function btrProject_diffs_content($origin, $project, $lng) {
  $form = [
    'project_diffs' => [
      '#id' => 'btrproject-diffs',
      '#type' => 'fieldset',
      '#title' => t('Project Diffs'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,

      'diff_list' => btrProject_diffs_table($origin, $project, $lng),

      'advanced' => [
        '#prefix' => '<div class="row">',
        '#suffix' => '</div>',
        '#access' =>  bcl::user_is_project_admin($origin, $project, $lng),

        'diff_latest' => [
          '#prefix' => '<div class="col-sm-6 col-sm-push-6">',
          drupal_get_form('btrProject_diffs_latest_form', $origin, $project, $lng),
          '#suffix' => '</div>',
        ],

        'make_snapshot' => [
          '#prefix' => '<div class="col-sm-6 col-sm-pull-6">',
          drupal_get_form('btrProject_diffs_snapshot_form', $origin, $project, $lng),
          '#suffix' => '</div>',
        ],
      ],
    ],
  ];

  return $form;
}

/**
 * Return the structure of the table that has the list of the diffs.
 */
function btrProject_diffs_table($origin, $project, $lng) {
  $btr_server = variable_get('btrClient_server');
  $url_diff = "$btr_server/project/diff/$origin/$project/$lng";
  $url_diff_dl = "$btr_server/project/diff-dl/$origin/$project/$lng";
  $output = bcl::http_request($url_diff);
  $lines = explode("\n", $output->data);
  $rows = array();
  foreach ($lines as $line) {
    list($nr, $time, $comment) = explode("\t", $line, 3);
    $links = drupal_render(_btrProject_diffs_links($url_diff_dl, $nr));
    $rows[] = array($time, $comment, $links);
  }
  $table = [
    '#theme' => 'table',
    '#header' => ['time', 'comment', 'get'],
    '#rows' => $rows,
  ];
  return $table;
}

/**
 * Return the links for downloading a diff.
 */
function _btrProject_diffs_links($url_diff_dl, $nr) {
  $buttons['get_diff'] = [
    '#markup' => l('diff', "$url_diff_dl/$nr", [
                 'external' => TRUE,
                 'absolute' => TRUE,
                 'attributes' => [
                   'class' => ['btn', 'btn-default'],
                 ]]),
  ];
  $buttons['get_ediff'] = [
    '#markup' => l('ediff', "$url_diff_dl/$nr/ediff", [
                 'external' => TRUE,
                 'absolute' => TRUE,
                 'attributes' => [
                   'class' => ['btn', 'btn-default'],
                   'style' => 'margin-left: 3px;',
                 ]]),
  ];

  return $buttons;
}

/**
 * Form callback for: btrProject_diffs_latest_form
 *
 * Get the diffs between the latest snapshot and the current version.
 */
function btrProject_diffs_latest_form($form, &$form_state, $origin, $project, $lng) {
  $form = [
    'latest_diffs' => [
      '#type' => 'fieldset',
      '#title' => t('Get the latest diffs'),
      '#description' => t('Get the diffs between the latest snapshot and the current version.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#access' => bcl::user_is_project_admin($origin, $project, $lng),

      'diff_export_mode' => [
        '#type' => 'select',
        '#title' => t('Export mode'),
        '#description' => t('The export mode that should be used.'),
        '#options' => [
          'original' => t('Export the original translations that were imported.'),
          'most_voted' => t('Export the most voted translations'),
          'preferred_by_me' => t('Prefer translations voted by me'),
          'preferred_by_my_friends' => t('Prefer translations voted by my friends'),
        ],
        '#default_value' => 'most_voted',
      ],

      'friends' => [
        '#type' => 'textarea',
        '#title' => t('List of friends'),
        '#description' => t('Enter usernames of your friends, separated by comma.'),
        '#states' => [
          'visible' => [
            ':input[name="diff_export_mode"]' => ['value' => 'preferred_by_my_friends'],
          ],
        ],
      ],

      'diff_format' => [
        '#type' => 'radios',
        '#title' => t('Diff format'),
        '#options' => [
          'diff' => 'diff',
          'ediff' => 'ediff',
        ],
        '#default_value' => 'diff',
      ],

      'submit' => [
        '#value' => t('Get'),
        '#type' => 'submit',
      ],
    ],
  ];

  return $form;
}

/**
 * Submit callback for the form: btrProject_diffs_latest_form
 */
function btrProject_diffs_latest_form_submit($form, &$form_state) {
  // Get the arguments origin/project/lng from the path.
  list($origin, $project, $lng) = bcl::get_project_from_path();

  // Allow only if user is project admin (for performance reasons).
  if (!bcl::user_is_project_admin($origin, $project, $lng)) {
    drupal_set_message(t('Only project admins can get the latest diffs.'), 'error');
    return;
  }

  // Get form values.
  $values = $form_state['values'];
  $export_mode = check_plain($values['diff_export_mode']);
  $friends = check_plain($values['friends']);
  $format = check_plain($values['diff_format']);

  // Request the latest diff.
  $btr_server = variable_get('btrClient_server');
  drupal_goto($btr_server . "/project/diff-latest/$origin/$project/$lng", [
      'absolute' => TRUE,
      'external' => TRUE,
      'query' => [
        'format' => $format,
        'export_mode' => $export_mode,
        'friends' => $friends,
      ],
    ]);
}

/**
 * Form callback for: btrProject_diffs_snapshot_form
 *
 * Make a snapshot of the current version of translations.
 */
function btrProject_diffs_snapshot_form($form, &$form_state, $origin, $project, $lng) {
  $form = [
    'make_snapshot' => [
      '#type' => 'fieldset',
      '#title' => t('Make a project snapshot'),
      '#description' => t('Make a snapshot of the current version of translations.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,

      'comment' => [
        '#type' => 'textarea',
        '#title' => t('Comment'),
        '#description' => t('A comment for the new snapshot.'),
      ],

      'snapshot_export_mode' => [
        '#type' => 'select',
        '#title' => t('Export mode'),
        '#description' => t('The export mode that should be used.'),
        '#options' => [
          'original' => t('Export the original translations that were imported.'),
          'most_voted' => t('Export the most voted translations'),
          'preferred_by_me' => t('Prefer translations voted by me'),
          'preferred_by_my_friends' => t('Prefer translations voted by my friends'),
        ],
        '#default_value' => 'most_voted',
      ],

      'friends' => [
        '#type' => 'textarea',
        '#title' => t('List of friends'),
        '#description' => t('Enter usernames of your friends, separated by comma.'),
        '#states' => [
          'visible' => [
            ':input[name="snapshot_export_mode"]' => ['value' => 'preferred_by_my_friends'],
          ],
        ],
      ],

      'submit' => [
        '#value' => t('Make'),
        '#type' => 'submit',
      ],
    ],
  ];

  return $form;
}

/**
 * Submit callback for the form: btrProject_diffs_snapshot_form
 */
function btrProject_diffs_snapshot_form_submit($form, &$form_state) {
  // Get the arguments origin/project/lng from the path.
  list($origin, $project, $lng) = bcl::get_project_from_path();

  // Allow only if user is project admin (for performance reasons).
  if (!bcl::user_is_project_admin($origin, $project, $lng)) {
    drupal_set_message(t('Only project admins can make a project snapshot.'), 'error');
    return;
  }

  // Get form values.
  $values = $form_state['values'];
  $comment = check_plain($values['comment']);
  $export_mode = check_plain($values['snapshot_export_mode']);
  $friends = check_plain($values['friends']);

  // Make project snapshot.
  $btr_server = variable_get('btrClient_server');
  $destination = url(request_uri(), ['absolute' => TRUE]);
  drupal_goto($btr_server . "/project/snapshot/$origin/$project/$lng", [
      'absolute' => TRUE,
      'external' => TRUE,
      'query' => [
        'comment' => $comment,
        'export_mode' => $export_mode,
        'friends' => $friends,
        'dest' => $destination,
      ],
    ]);
}
