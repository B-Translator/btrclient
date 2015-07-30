<?php
/**
 * @file
 * Function: user_authenticate()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Authenticate the user (redirect to login).
 * But first save the given form_state in session.
 */
function user_authenticate($form_state, $redirection = FALSE) {
  if ($redirection) {
    // We are in a redirect-after-login, but login has failed or was cancelled.
    // In this case we clear the session variable so that it does not keep
    // redirecting.
    unset($_SESSION['btrClient']['form_state']);
    return;
  }

  $_SESSION['btrClient']['form_state'] = $form_state;

  if (bcl::installed_on_server()) {
    drupal_goto('user/login', array('query' => drupal_get_destination()));
  }
  else {
    oauth2_login();
  }
}
