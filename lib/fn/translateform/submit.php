<?php
/**
 * @file
 * Function: translateform_submit()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Submit the translate form.
 */
function translateform_submit($form, $form_state) {
  $op = $form_state['values']['op'];
  if ($op == t('Login')) {
    bcl::user_authenticate($form_state, $redirection = $form===NULL);
  }
  elseif ($op == t('Save')) {
    if (bcl::user_is_authenticated()) {
      bcl::translateform_save($form_state['values']);
    }
    else {
      bcl::user_authenticate($form_state, $redirection = $form===NULL);
    }
  }
}
