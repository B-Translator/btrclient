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
  else {
    return bcl::user_has_oauth2_token();
  }
}
