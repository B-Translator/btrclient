<?php
/**
 * @file
 * Function: translateform_submit()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Submit callback for the form: btrClient_vote_form
 */
function translateform_submit($form, &$form_state) {
  $op = $form_state['values']['op'];
  if ($op == t('Login')) {
    bcl::user_authenticate($form, $form_state);
  }
  elseif ($op == t('Save')) {
    if (bcl::user_is_authenticated()) {
      bcl::translateform_save($form_state['values']);
    }
    else {
      bcl::user_authenticate($form, $form_state);
    }
  }
}
