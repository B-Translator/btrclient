<?php
/**
 * @file
 * Callback for menu item: 'btr/project/%/%/%/admin'
 */

/**
 * Menu callback for 'btr/project/%/%/%/admin'
 */
function btrProject_admin($origin, $project, $lng) {
  $output[] = drupal_get_form('btrProject_admin_sync', $origin, $project, $lng);
  return $output;
}

/**
 * Form callback for: btrProject_admin_sync
 */
function btrProject_admin_sync($form, &$form_state, $origin, $project, $lng) {
  $btr_server = variable_get('btrClient_server', 'https://btranslator.org');
  $form['sync'] = array(
    '#access' =>  bcl::user_is_project_admin($origin, $project, $lng),
    '#markup' => l(t('Synchronize'), "$btr_server/project/sync",
               [
                 'external' => TRUE,
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
