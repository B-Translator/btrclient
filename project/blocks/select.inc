<?php
/**
 * @file
 * Functions for the block select.
 */

/**
 * Called from hook_block_view for select.
 */
function btrProject_select_view() {
  // Get the arguments from the path.
  $args = explode('/', current_path());
  if ($args[0]=='btr' and $args[1]=='project') {
    $origin = $args[2];
    $project = $args[3];
    $lng = $args[4];
  }
  else {
    $origin = $project = $lng = NULL;
  }

  $block['subject'] = t('Select a project');
  $block['content'] = drupal_get_form('btrProject_select_form', $origin, $project, $lng);
  return $block;
}

/**
 * Form callback for: btrProject_select_form
 */
function btrProject_select_form($form, &$form_state, $origin, $project, $lng) {
  $form['select_project'] = array(
    '#type' => 'ajax_chain_select',
    '#title' => t('Select a Project'),
    '#config' => [
      'origin' => [
        '#title' => t('Origin'),
        '#empty_option' => t('- Origin -'),
        '#default_value' => $origin,
      ],
      'project' => [
        '#title' => t('Project'),
        '#empty_option' => t('- Project -'),
        '#default_value' => $project,
      ],
      'lng' => [
        '#title' => t('Language'),
        '#empty_option' => t('- Language -'),
        '#default_value' => $lng,
      ],
    ],
    '#required_levels' => 3,
    '#show_throbber' => TRUE,
    '#progress_message' => t('Please wait..'),
    '#data_callback' => 'btrProject_select_fetchdata',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Go'),
  );

  return $form;
}

/**
 * Data callback for form item:  btrProject_select_form/select_project
 */
function btrProject_select_fetchdata($level = 0, $previous_level_key = 0) {
  $btr_server = variable_get('btrClient_server');
  $data = array();
  switch ($level) {
    case 'origin':
      $result = drupal_http_request($btr_server . '/project/list/*/-/json');
      if ($result->code == 200) {
        $arr = drupal_json_decode($result->data);
        foreach ($arr as $origin) {
          $data[$origin] = $origin;
        }
      }
      break;

    case 'project':
      $origin = $previous_level_key;
      $result = drupal_http_request($btr_server . "/project/list/$origin/*/json");
      if ($result->code == 200) {
        $arr = drupal_json_decode($result->data);
        foreach ($arr as $item) {
          $project = preg_replace("#^$origin/#", '', $item);
          $data["$origin:$project"] = $project;
        }
      }
      break;

    case 'lng':
      $project = str_replace(':', '/', $previous_level_key);
      $result = drupal_http_request($btr_server . '/project/langs/' . $project);
      if ($result->code == 200) {
        $data = drupal_json_decode($result->data);
      }
      break;
  }

  return $data;
}

/**
 * Submit callback for the form: btrProject_select_form
 */
function btrProject_select_form_submit($form, &$form_state) {
  $values = $form_state['values']['select_project'];
  list($origin, $project) = explode(':', $values['project'], 2);
  $lng = $values['lng'];

  drupal_goto("/btr/project/$origin/$project/$lng/dashboard");
}
