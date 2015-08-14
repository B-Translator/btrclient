<?php
/**
 * @file
 * Callback for menu item: 'btr/project/%/%/%/admin'
 */

/**
 * Menu callback for 'btr/project/%/%/%/admin'
 */
function btrProject_admin($origin, $project, $lng) {
  $output[] = drupal_get_form('btrProject_admin_form', $origin, $project, $lng);
  return $output;
}

/**
 * Form callback for: btrProject_admin_form
 */
function btrProject_admin_form($form, &$form_state, $origin, $project, $lng) {
  $btr_server = variable_get('btrClient_server', 'https://btr.example.org');

  // The admin interface works well only on the B-Translator server.
  if (!bcl::installed_on_server()) {
    drupal_goto("$btr_server/btr/project/$origin/$project/$lng/admin");
    return;
  }

  $form['sync'] = array(
    '#access' =>  btr::user_is_project_admin($origin, $project, $lng),
    '#markup' => l(t('Synchronize'), "project/sync",
               [
                 'attributes' => [
                   'class' => ['btn', 'form-submit'],
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
