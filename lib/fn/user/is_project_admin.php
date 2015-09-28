<?php
/**
 * @file
 * Definition of function user_is_project_admin().
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Return TRUE if the oauth2 user can administrate the given project.
 */
function user_is_project_admin($origin, $project, $lng = NULL) {
  // If user has global admin permission,
  // he can administrate this project as well.
  if (bcl::user_access('btranslator-admin'))  return TRUE;

  // Check that the project language matches translation_lng of the user.
  $btr_user = oauth2_user_get();
  if ($lng !== NULL and $lng != $btr_user['translation_lng']) return FALSE;

  // Check whether the user is an admin of the given project.
  if (in_array("$origin/$project", $btr_user['admin_projects'])) return TRUE;

  // Otherwise he cannot administrate.
  return FALSE;
}
