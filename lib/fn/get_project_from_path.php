<?php
/**
 * @file
 * Function: get_project_from_path()
 */

namespace BTranslator\Client;

/**
 * Extract the $origin, $project and $lng from the current path,
 * if it is in the form: /btr/project/$origin/$project/$lng/...
 */
function get_project_from_path() {
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

  return [$origin, $project, $lng];
}
