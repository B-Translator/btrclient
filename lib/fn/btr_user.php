<?php
/**
 * @file
 * Get the B-Translator user data.
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Get the B-Translator user.
 */
function btr_user_get() {
  // If it is already cached, return the cached one.
  if (isset($_SESSION['btrClient']['btr_user'])) {
    return $_SESSION['btrClient']['btr_user'];
  }
  else {
    return btr_user_get_from_server();
  }
}

/**
 * Get the B-Translator user from the B-Translator server.
 */
function btr_user_get_from_server() {
  if (! bcl::user_is_authenticated())  return NULL;

  $btr = wsclient_service_load('btr');
  $data = $btr->user_profile();

  btr_user_set($data);
  return  $_SESSION['btrClient']['btr_user'];
}

/**
 * Set the B-Translator user from the given array.
 */
function btr_user_set($data) {
  // Get the profile fields.
  $btr_user = array(
    'translation_lng' => $data['translation_lng'],
    'string_order' => $data['string_order'],
    'preferred_projects' => $data['preferred_projects'],
    'auxiliary_languages' => $data['auxiliary_languages'],
    'translations_per_day' => $data['translations_per_day'],
    'feedback_channels' => $data['feedback_channels'],
    'permissions' => $data['permissions'],
    'uid' => $data['identifier'],
    'name' => $data['displayName'],
  );

  $_SESSION['btrClient']['btr_user'] = $btr_user;
}

/**
 * Remove btr_user from the session.
 */
function btr_user_expire() {
  unset($_SESSION['btrClient']['btr_user']);
}
