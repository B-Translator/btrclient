<?php
/**
 * @file
 * Misc utility functions.
 */

/**
 * Unpacks a string as retrieved from the database.
 *
 * Creates an array out of the string. If it was a single string, the array
 * will have one item. If the string was a plural string, the array will have
 * as many items as the language requires (two for source strings).
 *
 * @param string $string
 *   The string with optional separation markers (NULL bytes).
 *
 * @return array
 *   An array of strings with one element for each plural form in case of
 *   a plural string, or one element in case of a regular string. This
 *  is called a $textarray elsewhere.
 */
function btrClient_unpack_string($string) {
  return explode("\0", $string);
}

/**
 * Packs a string for storage in the database.
 *
 * @param array $strings
 *   An array of strings.
 *
 * @return string
 *   A packed string with NULL bytes separating each string.
 */
function btrClient_pack_string($strings) {
  if (is_array($strings)) {
    return implode("\0", $strings);
  }
  else {
    return $strings;
  }
}

/**
 * Check whether the current user has the given permission (on the server).
 *
 * @param string $permission
 *   The name of the permission (like 'btranslator-*').
 *
 * @return bool
 *   TRUE or FALSE
 */
function btr_user_access($permission) {
  // If the user is not logged in, he doesn't have any permissions.
  if (!user_is_logged_in()) {
    return FALSE;
  }

  // If the user has not been authenticated yet by oauth2,
  // he doesn't have any permissions.
  if (!btr_user_is_authenticated()) {
    return FALSE;
  }

  // Check the given permission on the list of permissions.
  $btr_profile = btr_get_user_profile();
  return in_array($permission, $btr_profile['permissions']);
}

/**
 * Return true if there is an oauth2 access_token.
 */
function btr_user_is_authenticated() {
  if (_btrClient_installed_on_server()) {
    return user_is_logged_in();
  }

  $server_url = variable_get('oauth2_login_oauth2_server', '');
  $token_endpoint = $server_url . '/oauth2/token';
  $client_id = variable_get('oauth2_login_client_id', '');
  $auth_flow = 'server-side';

  // Get the current access_token.
  $id = md5($token_endpoint . $client_id . $auth_flow);
  $token = oauth2_client_get_token($id);

  // Check the access token.
  $authenticated = !empty($token['access_token']);
  return $authenticated;
}

/**
 * Return true if btrClient is installed on B-Translator server.
 */
function _btrClient_installed_on_server() {
  return module_exists('btrCore');
}

/**
 * Return the profile of the user on B-Translator server.
 */
function btr_get_user_profile() {
  // Return the cached one, if possible.
  if (isset($_SESSION['btrClient']['btr_profile'])) {
    return $_SESSION['btrClient']['btr_profile'];
  }

  // Get the profile from the B-Translator server.
  $btr_profile = _btrClient_get_user_profile();

  // Save it in session.
  $_SESSION['btrClient']['btr_profile'] = $btr_profile;

  return $btr_profile;
}

/**
 * Get and return the remote profile of the user.
 */
function _btrClient_get_user_profile($btr_user = NULL) {
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
  );

  return $btr_profile;
}


/**
 * Display any messages, warnings and errors.
 *
 * @param array $messages
 *   Array of messages, usually returned from btr rest services.
 *   Each item is an array that contains a string message and a type,
 *   where type can be one of: status, warning, error.
 */
function btrClient_display_messages($messages) {
  foreach ($messages as $message) {
    list($msg, $type) = $message;
    if (!in_array($type, array('status', 'warning', 'error'))) {
      $type = 'status';
    }
    drupal_set_message($msg, $type);
  }
}


/**
 * Return an array of languages and their details.
 */
function btrClient_get_languages() {
  $cache = cache_get('btrClient_languages', 'cache_btrClient');
  // Return cache if possible.
  if (!empty($cache) && isset($cache->data) && !empty($cache->data)) {
    return $cache->data;
  }

  // Get an array of languages from the server.
  if (_btrClient_installed_on_server()) {
    $languages = btr_get_languages();
  }
  else {
    $btr_server = variable_get('btrClient_languages', 'https://dev.btranslator.org');
    $output = drupal_http_request("$btr_server/translations/languages");
    $languages = json_decode($output->data, TRUE);
  }

  // Cache until a general cache wipe.
  cache_set('btrClient_languages', $languages, 'cache_btrClient', CACHE_TEMPORARY);

  return $languages;
}

/**
 * Return the language of translations.
 */
function btrClient_get_translation_lng() {
  $lng = variable_get('btrClient_translation_lng', 'fr');
  if ($lng == 'all') {
    if (!btr_user_is_authenticated()) {
      $lng = 'fr';
    }
    else {
      $btr_profile = btr_get_user_profile();
      $lng = $btr_profile['translation_lng'];
    }
  }
  return $lng;
}

/**
 * Shorten the given string.
 *
 * From the given (possibly long) string, returns a short string
 * of the given length that can be suitable for title, subject, etc.
 */
function btrClient_shorten($string, $length) {
  $str = strip_tags(str_replace("\n", ' ', $string));
  if (strlen($str) > $length) {
    $str = substr($str, 0, strrpos(substr($str, 0, $length - 3), ' '));
    $str .= '...';
  }
  $str = utf8_decode($str);
  $str = htmlentities($str);
  return $str;
}
