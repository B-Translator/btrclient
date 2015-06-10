<?php
/**
 * @file
 * Function: form_resubmit()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * If there is a $_SESSION['btrClient']['form_state'],
 * then this is a redirect after login. Call the submit function again.
 */
function form_resubmit() {
  if (isset($_SESSION['btrClient']['form_state'])) {
    $form_state = $_SESSION['btrClient']['form_state'];
    unset($_SESSION['btrClient']['form_state']);
    $form_id = $form_state['values']['form_id'];
    $submit = $form_id . '_submit';
    if (function_exists($submit)) {
      $submit(NULL, $form_state);
    }
  }
}
