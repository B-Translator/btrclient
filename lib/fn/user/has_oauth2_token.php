<?php
/**
 * @file
 * Function: user_has_oauth2_token()
 */

namespace BTranslator\Client;

/**
 * Return true if the user has an oauth2 access_token.
 */
function user_has_oauth2_token() {
  $server_url = variable_get('oauth2_login_oauth2_server', '');
  $token_endpoint = $server_url . '/oauth2/token';
  $client_id = variable_get('oauth2_login_client_id', '');
  $auth_flow = 'server-side';

  // Get the current access_token.
  $id = md5($token_endpoint . $client_id . $auth_flow);
  $token = oauth2_client_get_token($id);

  // Check the access token.
  return !empty($token['access_token']);
}
