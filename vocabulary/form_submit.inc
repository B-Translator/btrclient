<?php
/**
 * @file
 * Submit functions for btrVocabulary_form.
 */

/**
 * Submit callback for: btrVocabulary_form
 */
function btrVocabulary_form_submit($form, &$form_state) {
  $form_values = $form_state['values'];
  $op = $form_values['op'];

  if ($op == t('Go')) {
    btrVocabulary_form_go($form_state);
  }
  elseif ($op == t('Login')) {
    _btrVocabulary_form_login($form, $form_state);
  }
  elseif ($op == t('Save')) {
    _btrVocabulary_form_save($form, $form_state);
  }
  elseif ($op == t('Next')) {
    $form_state['values']['search_term'] = '';
    btrVocabulary_form_go($form_state);
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
 * Go to the selected term.
 */
function btrVocabulary_form_go($form_state) {
  $form_values = $form_state['values'];
  $term = $form_values['search_term'];
  $vocabulary = $form_values['vocabulary'];
  $query = $form_values['get_query'];
  unset($query['q']);
  $url = "vocabulary/$vocabulary";
  if (trim($term) != '') {
    $url .= "/$term";
  }
  // Redirect.
  drupal_goto($url, array('query' => $query));
}

/**
 * The button 'Login' has been clicked.
 */
function _btrVocabulary_form_login($form, $form_state) {
  if (bcl::user_is_authenticated())  return;
  bcl::user_authenticate($form_state, $redirection = $form===NULL);
}

/**
 * The button 'Save' has been clicked.
 */
function _btrVocabulary_form_save($form, $form_state) {
  // Make sure that user is authenticated.
  if (!bcl::user_is_authenticated()) {
    bcl::user_authenticate($form_state, $redirection = $form===NULL);
    return;
  }

  // Save the values selected on the form.
  bcl::translateform_save($form_state['values']);

  // Redirect to the same term.
  btrVocabulary_form_go($form_state);
}

/**
 * The button 'Add Term' has been clicked.
 */
function _btrVocabulary_form_add($form, $form_state) {
  // Make sure that user is authenticated.
  if (!bcl::user_is_authenticated()) {
    bcl::user_authenticate($form_state, $redirection = $form===NULL);
  }

  // Check access rights.
  if (!bcl::user_access('btranslator-import')) {
    drupal_set_message(t("You don't have enough rights for adding new terms."), 'error');
    return;
  }

  // Add the new term.
  try {
    $term =  $form_state['values']['search_term'];
    $vocabulary =  $form_state['values']['vocabulary'];
    $btr = wsclient_service_load('btr');
    $result = $btr->add_string(array(
                'origin' => 'vocabulary',
                'project' => $vocabulary,
                'tplname' => $vocabulary,
                'string' => $term,
                'context' => $vocabulary,
                'notify' => TRUE,
              ));
    if ($result['sguid']) {
      // Redirect to the new term.
      btrVocabulary_form_go($form_state);
    }
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage(), 'error');
  }
}

/**
 * The button 'Delete Term' has been clicked.
 */
function _btrVocabulary_form_delete($form, $form_state) {
  // Make sure that user is authenticated.
  if (!bcl::user_is_authenticated()) {
    bcl::user_authenticate($form_state, $redirection = $form===NULL);
  }

  // Check access rights.
  if (!bcl::user_access('btranslator-admin')) {
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
    btrVocabulary_form_go($form_state);
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage(), 'error');
  }
}
