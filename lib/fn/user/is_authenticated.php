<?php
/**
 * @file
 * Function: user_is_authenticated()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Return true if there is an oauth2 access_token.
 */
function user_is_authenticated() {
  if (bcl::installed_on_server()) {
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
