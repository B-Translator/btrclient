<?php
/**
 * @file
 * AJAX callback functions.
 */

/**
 * Return string context, a list of related projects (where the string appears), etc.
 * This callback is invoked from JavaScript and is used as an AJAX provider.
 *
 * @param $lng
 *   Language (used for the links).
 *
 * @param $sguid
 *   Id of the string.
 *
 * @return
 *   HTML code with the details of the given string.
 */
function btrClient_string_details($lng, $sguid) {
  // Get the string details from the server.
  $url = variable_get('btrClient_server') . '/ajax/string_details/' . $sguid;
  $output = drupal_http_request($url);
  if (isset($output->data)) {
    $details = json_decode($output->data, TRUE);
  }
  else {
    $details = [
      'context' => '',
      'projects' => [],
    ];
  }

  $output = '';

  // The context of this string.
  if (!empty($details['context'])) {
    $output .= '<h3>' . t('Used with context: %context', ['%context' => $details['context']]) . '</h3>';
  }

  // List the projects where this string is used.
  $output .= '<h3>' . t('Used on these projects:') . '</h3>';
  $project_list = array();
  foreach ($details['projects'] as $project) {
    $url = $GLOBALS['base_url'] . '/btr/project/' . $project . '/' . $lng . '/' . $sguid;
    $project_list[] = l($project, $url);
  }
  $output .= theme('item_list', array('items' => $project_list));

  print $output;
  exit(0);
}
