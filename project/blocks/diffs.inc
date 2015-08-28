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
  $block['content'] = btrProject_diffs_table($origin, $project, $lng);
  //$block['content'] = drupal_get_form('btrProject_diffs_form', $origin, $project);
  return $block;
}

/**
 * Return the structure of the table that has the list of the diffs.
 */
function btrProject_diffs_table($origin, $project, $lng) {
  $btr_server = variable_get('btrClient_server');
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
                   //'style' => 'margin-right:3px;',
                   'target' => '_blank',
                 ]]),
  ];
  $buttons['get_ediff'] = [
    '#markup' => l('ediff', "$url_diff_dl/$nr/ediff", [
                 'external' => TRUE,
                 'absolute' => TRUE,
                 'attributes' => [
                   'class' => ['btn'],
                   //'style' => 'margin-right:3px;',
                   'target' => '_blank',
                 ]]),
  ];

  return $buttons;
}

/**
 * Form callback for: btrProject_diffs_form
 */
function btrProject_diffs_form($form, &$form_state, $origin, $project) {
  $form = [
    'origin' => [
      '#type' => 'hidden',
      '#value' => $origin,
    ],
    'project' => [
      '#type' => 'hidden',
      '#value' => $project,
    ],
    'subscribe' => [
      '#value' => t('Subscribe'),
      '#type' => 'submit',
      '#attributes' => ['class' => ['btn-primary']],
      '#access' => !$subscribed,
    ],
    'unsubscribe' => [
      '#value' => t('Unsubscribe'),
      '#type' => 'submit',
      '#attributes' => ['class' => ['btn-primary']],
      '#access' => $subscribed,
    ],
  ];

  return $form;
}

/**
 * Submit callback for the form: btrProject_diffs_form
 */
function btrProject_diffs_form_submit($form, &$form_state) {
  // If user is not authenticated, redirect to login first.
  if (!bcl::user_is_authenticated()) {
    bcl::user_authenticate($form_state, $redirection = ($form===NULL));
    return;
  }

}
