<?php
/**
 * @file
 * Subscribe to a project.
 */

/**
 * Called from hook_block_view for subscribe.
 */
function btrProject_subscribe_view() {
  // Get the arguments from the path.
  list($origin, $project, $lng) = bcl::get_project_from_path();

  $block['subject'] = t('Subscribe to Project');
  $block['content'] = drupal_get_form('btrProject_subscribe_form', $origin, $project);
  return $block;
}

/**
 * Form callback for: btrProject_subscribe_form
 */
function btrProject_subscribe_form($form, &$form_state, $origin, $project) {
  $subscribed = _btrProject_user_subscribed($origin, $project);
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
      '#value' => '<span class="glyphicon glyphicon-envelope"></span> - ' . t('Subscribe'),
      '#type' => 'submit',
      '#attributes' => ['class' => ['btn-primary']],
      '#access' => !$subscribed,
    ],
    'unsubscribe' => [
      '#value' => '<span class="glyphicon glyphicon-envelope"></span> - ' . t('Unsubscribe'),
      '#type' => 'submit',
      '#attributes' => ['class' => ['btn-primary']],
      '#access' => $subscribed,
    ],
  ];

  return $form;
}

/**
 * Return TRUE if the user is subscribed to the given project.
 */
function _btrProject_user_subscribed($origin, $project) {
  if (!oauth2_user_is_authenticated()) return FALSE;

  // Check the given project on the list of projects.
  $btr_user = oauth2_user_get();
  if (!isset($btr_user['projects']))  return FALSE;
  return in_array("$origin/$project", $btr_user['projects']);
}

/**
 * Submit callback for the form: btrProject_subscribe_form
 */
function btrProject_subscribe_form_submit($form, &$form_state) {
  // If user is not authenticated, redirect to login first.
  if (!oauth2_user_is_authenticated()) {
    oauth2_user_authenticate($form_state, $redirection = ($form===NULL));
    return;
  }

  // Get the parameters.
  $values = $form_state['values'];
  $operation = $values['op'];
  $origin = $values['origin'];
  $project = $values['project'];

  $btr = wsclient_service_load('btr');
  if ($operation == '<span class="glyphicon glyphicon-envelope"></span> - ' . t('Subscribe')) {
    // Subscribe to the project.
    $btr->subscribe(['origin' => $origin, 'project' => $project]);
    $msg = t("Subscribed to project '!project'. You will receive daily emails from this project.",
           ['!project' => "$origin/$project"]);
  }
  elseif ($operation == '<span class="glyphicon glyphicon-envelope"></span> - ' . t('Unsubscribe')) {
    // Unsubscribe from the project.
    $btr->unsubscribe(['origin' => $origin, 'project' => $project]);
    $msg = t("Unsubscribed from project '!project'. You will not receive daily emails from this project.",
           ['!project' => "$origin/$project"]);
  }
  drupal_set_message($msg);

  // Expire from cache the btr_user, so that it can be
  // refreshed with the latest values.
  oauth2_user_expire();
}
