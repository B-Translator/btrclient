<?php
/**
 * @file
 * Definition of function user_is_project_moderator().
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Return TRUE if the oauth2 user can moderate the given project.
 */
function user_is_project_moderator($origin, $project, $lng = NULL) {
  // If user has admin permissions, he can also moderate this project.
  if (bcl::user_is_project_admin($origin, $project, $lng))  return TRUE;

  // Check that the project language matches translation_lng of the user.
  $btr_user = oauth2_user_get();
  if ($lng !== NULL and $lng != $btr_user['translation_lng']) return FALSE;

  // Check whether the user is an moderator of the given project.
  if (in_array("$origin/$project", $btr_user['moderate_projects'])) return TRUE;

  // Otherwise he cannot moderate.
  return FALSE;
}
