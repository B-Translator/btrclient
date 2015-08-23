<?php
/**
 * @file
 * Functions for the block 'list', which displays a list of the available projects.
 */

/**
 * Called from hook_block_view for list.
 */
function btrProject_list_view() {
  $block['subject'] = t('List of Projects');
  $block['content'] = array('#markup' => btrProject_list_view_html());
  //$block['content'] = drupal_get_form('btrProject_list_form');
  return $block;
}

/**
 * Return the html body of the block.
 */
function btrProject_list_view_html() {

  $btr_server = variable_get('btrClient_server');
  $result = drupal_http_request($btr_server . '/project/list/all');
  if ($result->code != 200) {
    drupal_set_message(t('Could not fetch data from the server.'));
    return '';
  }

  $html = "<ul class='collapsibleList'>\n";
  $project_list = drupal_json_decode($result->data);
  foreach ($project_list as $origin => $arr_project) {
    $html .= "
      <li>
        <label for='$origin'>$origin</label>
        <input type='checkbox' id='$origin' />
        <ul>";
    foreach ($arr_project as $project => $arr_lng) {
      $html .= "
        <li>
          <label for='$origin-$project'>$project</label>
          <input type='checkbox' id='$origin-$project' />
          <ul>";
      foreach ($arr_lng as $lng) {
        $html .= "
            <li>
              <a href='/btr/project/$origin/$project/$lng/dashboard'>$origin/$project/$lng</a>
            </li>";
      }
      $html .= "
          </ul>
        </li>";
    }
    $html .= "
      </ul>
    </li>";
  }

  return $html;
}

/**
 * Form callback for: btrProject_list_form
 */
function btrProject_list_form($form, &$form_state) {
  $form = [
    'project_list' => [
      '#type' => 'fieldset',
      '#title' => t('Select a Project'),
    ],
  ];

  $btr_server = variable_get('btrClient_server');
  $result = drupal_http_request($btr_server . '/project/list/all');
  if ($result->code != 200) {
    drupal_set_message(t('Could not fetch data from the server.'));
    return $form;
  }

  $project_list = drupal_json_decode($result->data);
  foreach ($project_list as $origin => $arr_project) {
    $form['project_list'][$origin] = array(
      '#type' => 'fieldset',
      '#title' => $origin,
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    foreach ($arr_project as $project => $arr_lng) {
      $form['project_list'][$origin][$project] = array(
        '#type' => 'fieldset',
        '#title' => $project,
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        'items' => array(
          '#theme' => 'item_list',
          '#type' => 'ul',
          '#items' => array(),
        ),
      );
      $items = array();
      foreach ($arr_lng as $lng) {
        $items[] = l("$origin/$project/$lng", "/btr/project/$origin/$project/$lng/dashboard");
      }
      $form['project_list'][$origin][$project]['items']['#items'] = $items;
    }
  }

  return $form;
}
