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
  // $url = "$btr_server/project/synch";
  $form['synch'] = array(
    '#markup' => l(t('Synchronize'), "$btr_server/project/synch",
               [
                 'external' => TRUE,
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
