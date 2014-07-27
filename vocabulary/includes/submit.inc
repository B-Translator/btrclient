<?php
/**
 * @file
 * Submit functions for the form btrVocabulary.
 */

/**
 * Form submit callback for btrVocabulary_form().
 */
function btrVocabulary_form_submit($form, &$form_state) {
  $form_values = $form_state['values'];
  $op = $form_values['op'];

  if ($op == t('Save')) {
    _btrVocabulary_form_save($form, $form_state);
  }
  elseif ($op == t('Next')) {
    $vocabulary = $form_values['vocabulary'];
    drupal_goto("vocabulary/$vocabulary");
  }
  else {
    // Do nothing.
  }
}

/**
 * The button 'Save' has been clicked.
 */
function _btrVocabulary_form_save($form, &$form_state) {
  if (btr_user_is_authenticated()) {
    // Save the values selected on the form.
    $form_values = $form_state['values'];
    _btrClient_translate_form_save($form_values);

    // Redirect to the same term.
    $term = $form_values['term'];
    $vocabulary = $form_values['vocabulary'];
    drupal_goto("vocabulary/$vocabulary/$term");
  }
  else {
    // User is not authenticated, login first (redirect to login).
    if (_btrClient_installed_on_server()) {
      drupal_goto('user/login', array('query' => drupal_get_destination()));
    }
    else {
      $_SESSION['btrVocabulary']['form_state'] = $form_state;
      oauth2_login();
    }
  }
}
