<?php
/**
 * @file
 * Function: user_get_remote_profile()
 */

namespace BTranslator\Client;

/**
 * Get and return the remote profile of the user.
 */
function user_get_remote_profile($btr_user = NULL) {
  if ($btr_user == NULL) {
    $btr = wsclient_service_load('btr');
    $btr_user = $btr->user_profile();
  }

  // Get the profile fields.
  $btr_profile = array(
    'translation_lng' => $btr_user['translation_lng'],
    'string_order' => $btr_user['string_order'],
    'preferred_projects' => $btr_user['preferred_projects'],
    'auxiliary_languages' => $btr_user['auxiliary_languages'],
    'translations_per_day' => $btr_user['translations_per_day'],
    'feedback_channels' => $btr_user['feedback_channels'],
    'permissions' => $btr_user['permissions'],
    'identifier' => $btr_user['uid'],
    'displayName' => $btr_user['name'],
  );

  return $btr_profile;
}
