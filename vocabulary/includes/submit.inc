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
  elseif ($op == t('Add Term')) {
    _btrVocabulary_form_add($form, $form_state);
  }
  elseif ($op == t('Delete Term')) {
    _btrVocabulary_form_delete($form, $form_state);
  }
  else {
    // Do nothing.
  }
}

/**
 * The button 'Save' has been clicked.
 */
function _btrVocabulary_form_save($form, &$form_state) {
  // Make sure that user is authenticated.
  if (!btr_user_is_authenticated()) {
    _btrVocabulary_authenticate($form_state);
  }

  // Save the values selected on the form.
  $form_values = $form_state['values'];
  _btrClient_translate_form_save($form_values);

  // Redirect to the same term.
  $term = $form_values['term'];
  $vocabulary = $form_values['vocabulary'];
  drupal_goto("vocabulary/$vocabulary/$term");
}
/**
 * Authenticate the user (redirect to login).
 */
function _btrVocabulary_authenticate($form_state) {
  if (_btrClient_installed_on_server()) {
    drupal_goto('user/login', array('query' => drupal_get_destination()));
  }
  else {
    $_SESSION['btrVocabulary']['form_state'] = $form_state;
    oauth2_login();
  }
}

/**
 * The button 'Add Term' has been clicked.
 */
function _btrVocabulary_form_add($form, &$form_state) {
  // Make sure that user is authenticated.
  if (!btr_user_is_authenticated()) {
    _btrVocabulary_authenticate($form_state);
  }

  // Check access rights.
  if (!btr_user_access('btranslator-import')) {
    drupal_set_message(t("You don't have enough rights for adding new terms."), 'error');
    return;
  }

  // Add the new term.
  $term =  $form_state['values']['search_term'];
  $vocabulary =  $form_state['values']['vocabulary'];
  $btr = wsclient_service_load('btr');
  try {
    $result = $btr->add_string(array(
                'origin' => 'vocabulary',
                'project' => $vocabulary,
                'tplname' => $vocabulary,
                'string' => $term,
                'context' => 'vocabulary',
                'notify' => TRUE,
              ));
    if ($result['sguid']) {
      // Redirect to the new term.
      drupal_goto("vocabulary/$vocabulary/$term");
    }
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage(), 'error');
  }
}

/**
 * The button 'Delete Term' has been clicked.
 */
function _btrVocabulary_form_delete($form, &$form_state) {
  // Make sure that user is authenticated.
  if (!btr_user_is_authenticated()) {
    _btrVocabulary_authenticate($form_state);
  }

  // Check access rights.
  if (!btr_user_access('btranslator-admin')) {
    drupal_set_message(t('Only an admin can delete vocabulary terms.'), 'error');
    return;
  }

  // Get the sguid of the string;
  foreach ($form_state['values']['strings'] as $sguid => $string) {
    break;
  }

  // Delete the term.
  $btr = wsclient_service_load('btr');
  try {
    $term =  $form_state['values']['search_term'];
    $btr->del_string(array('sguid' => $sguid));
    drupal_set_message(t('Term "!term" was deleted.', array('!term' => $term)));
    $vocabulary =  $form_state['values']['vocabulary'];
    drupal_goto("vocabulary/$vocabulary");
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage(), 'error');
  }
}
