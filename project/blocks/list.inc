<?php
/**
 * @file
 * Functions for the block 'list', which displays a list of the available projects.
 */

/**
 * Called from hook_block_view for list.
 */
function btrProject_list_view() {
  // Build the block.
  $block['subject'] = t('List of Projects');
  $block['content'] = array('#markup' => btrProject_list_view_html());

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
