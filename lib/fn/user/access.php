<?php
/**
 * @file
 * Function: user_access()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Check whether the current user has the given permission (on the server).
 *
 * @param string $permission
 *   The name of the permission (like 'btranslator-*').
 *
 * @return bool
 *   TRUE or FALSE
 */
function user_access($permission) {
  // If the user is not logged in, he doesn't have any permissions.
  if (!user_is_logged_in()) {
    return FALSE;
  }

  // If the user has not been authenticated yet by oauth2,
  // he doesn't have any permissions.
  if (!bcl::user_is_authenticated()) {
    return FALSE;
  }

  // Check the given permission on the list of permissions.
  $btr_user = bcl::btr_user_get();
  return in_array($permission, $btr_user['permissions']);
}
