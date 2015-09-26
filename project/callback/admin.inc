<?php
/**
 * @file
 * Callback for menu item: 'btr/project/%/%/%/admin'
 */

/**
 * Menu callback for 'btr/project/%/%/%/admin'
 */
function btrProject_admin($origin, $project, $lng) {
  // The admin interface works well only on the B-Translator server.
  if (!oauth2_user_installed_on_server()) {
    $btr_server = variable_get('btrClient_server', 'https://btr.example.org');
    drupal_goto("$btr_server/btr/project/$origin/$project/$lng/admin");
    return;
  }

  $output[] = drupal_get_form('btrProject_admin_sync', $origin, $project, $lng);

  return $output;
}

/**
 * Form callback for: btrProject_admin_sync
 */
function btrProject_admin_sync($form, &$form_state, $origin, $project, $lng) {
  $form['sync'] = array(
    '#access' =>  btr::user_is_project_admin($origin, $project, $lng),
    '#markup' => l(t('Synchronize'), "project/sync",
               [
                 'attributes' => [
                   'class' => ['btn', 'btn-default', 'form-submit'],
                   'target' => '_blank',
                 ],
                 'query' => [
                   'origin' => $origin,
                   'project' => $project,
                   'lng' => $lng,
                 ],
               ]),
  );

  return $form;
}
