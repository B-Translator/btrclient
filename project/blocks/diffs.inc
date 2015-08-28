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
  $block['content'][] = btrProject_diffs_table($origin, $project, $lng);
  $block['content'][] = drupal_get_form('btrProject_diffs_latest_form', $origin, $project, $lng);
  $block['content'][] = drupal_get_form('btrProject_diffs_snapshot_form', $origin, $project, $lng);
  return $block;
}

/**
 * Return the structure of the table that has the list of the diffs.
 */
function btrProject_diffs_table($origin, $project, $lng) {
  $btr_server = variable_get('btrClient_server');
  $btr_server = 'https://btranslator.org';
  $url_diff = "$btr_server/project/diff/$origin/$project/$lng";
  $url_diff_dl = "$btr_server/project/diff-dl/$origin/$project/$lng";
  $lines = file($url_diff);
  $rows = array();
  foreach ($lines as $line) {
    list($nr, $time, $comment) = explode("\t", $line, 3);
    $links = drupal_render(_btrProject_diffs_links($url_diff_dl, $nr));
    $rows[] = array($time, $comment, $links);
  }
  $table = [
    '#theme' => 'table',
    //'#attributes' => ['class' => ['l10n-table']],
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
                   'class' => ['btn'],
                 ]]),
  ];
  $buttons['get_ediff'] = [
    '#markup' => l('ediff', "$url_diff_dl/$nr/ediff", [
                 'external' => TRUE,
                 'absolute' => TRUE,
                 'attributes' => [
                   'class' => ['btn'],
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
  // Make the latest diffs only if the module is installed on the server and the
  // user is project admin.
  if ( !(bcl::installed_on_server() and btr::user_is_project_admin($origin, $project, $lng)) ) return '';

  $form = [
    'latest_diffs' => [
      '#type' => 'fieldset',
      '#title' => t('Get the latest diffs'),
      '#description' => t('Get the diffs between the latest snapshot and the current version.'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,

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
  // If user is not authenticated, redirect to login first.
  if (!bcl::user_is_authenticated()) {
    bcl::user_authenticate($form_state, $redirection = ($form===NULL));
    return;
  }
  // Allow only if installed on server and user is project admin.
  if ( !(bcl::installed_on_server() and btr::user_is_project_admin($origin, $project, $lng)) ) return;

  // Get the arguments origin/project/lng from the path.
  list($origin, $project, $lng) = bcl::get_project_from_path();

  $values = $form_state['values'];
  $export_mode = check_plain($values['diff_export_mode']);
  $friends = check_plain($values['friends']);

  if (in_array($export_mode, ['preferred_by_me', 'preferred_by_my_friends'])) {
    $export_mode = 'preferred';
  }

  // Get and check the list of preferred voters.
  if ($export_mode == 'preferred') {
    if (empty($preferred_voters)) {
      $preferred_voters = $GLOBALS['user']->name;
    }
    list($arr_emails, $error_messages) = btr::utils_get_emails($preferred_voters);
    if (!empty($error_messages)) {
      foreach ($error_messages as $msg) {
        drupal_set_message($msg, 'error');
      }
      return;
    }
  }

  // Export the latest most voted translations
  // and make the diffs with the last snapshot.
  $export_file = tempnam('/tmp', 'export_file_');
  $file_diff = tempnam('/tmp', 'file_diff_');
  $file_ediff = tempnam('/tmp', 'file_ediff_');
  btr::project_diff($origin, $project, $lng,
    $file_diff, $file_ediff, $export_file,
    $export_mode, $arr_emails);

  // Send the diff file.
  $fmt = check_plain($values['diff_format']);
  $file = ($fmt=='ediff' ? $file_ediff : $file_diff);
  $filename = "$origin-$project-$lng.$fmt";
  header('Content-Type: text/plain; charset=utf-8');
  header('Content-Disposition: attachment; filename="' . $filename . '"');
  header('Content-Transfer-Encoding: binary');
  header("Content-Length: " . filesize($file));
  $fp = fopen($file, 'rb');
  fpassthru($fp);
  fclose($fp);

  // Cleanup.
  unlink($export_file);
  unlink($file_diff);
  unlink($file_ediff);

  drupal_exit();
}

/**
 * Form callback for: btrProject_diffs_snapshot_form
 *
 * Make a snapshot of the current version of translations.
 */
function btrProject_diffs_snapshot_form($form, &$form_state, $origin, $project, $lng) {
  // Make the project snapshot only if the module is installed on the server and
  // the user is project admin.
  if ( !(bcl::installed_on_server() and btr::user_is_project_admin($origin, $project, $lng)) ) return '';

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
  // If user is not authenticated, redirect to login first.
  if (!bcl::user_is_authenticated()) {
    bcl::user_authenticate($form_state, $redirection = ($form===NULL));
    return;
  }
  // Allow only if installed on server and user is project admin.
  if ( !(bcl::installed_on_server() and btr::user_is_project_admin($origin, $project, $lng)) ) return;

  // Get the arguments origin/project/lng from the path.
  list($origin, $project, $lng) = bcl::get_project_from_path();

  $values = $form_state['values'];
  $export_mode = check_plain($values['diff_export_mode']);
  $friends = check_plain($values['friends']);

  if (in_array($export_mode, ['preferred_by_me', 'preferred_by_my_friends'])) {
    $export_mode = 'preferred';
  }

  // Get and check the list of preferred voters.
  if ($export_mode == 'preferred') {
    if (empty($preferred_voters)) {
      $preferred_voters = $GLOBALS['user']->name;
    }
    list($arr_emails, $error_messages) = btr::utils_get_emails($preferred_voters);
    if (!empty($error_messages)) {
      foreach ($error_messages as $msg) {
        drupal_set_message($msg, 'error');
      }
      return;
    }
  }

  $comment = $values['comment'];
  btr::project_snapshot($origin, $project, $lng, $comment, $export_mode, $arr_voters);
}
