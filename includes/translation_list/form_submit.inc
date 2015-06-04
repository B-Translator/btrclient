<?php
/**
 * @file
 * Translation form submission.
 * The form may contain more than one translations.
 */

/**
 * Form submit callback for btrClient_translate_form().
 *
 * @see btrClient_translate_form()
 */
function btrClient_translate_form_submit($form, &$form_state) {
  $form_values = $form_state['values'];

  // Check that the button clicked was either Save, or Next etc.
  $op = $form_values['op'];
  $operations = array(
    t('Login'),
    t('Save'),
    t('Next'),
    t('Translated'),
    t('Untranslated'),
  );
  if (!in_array($op, $operations)) {
    return;
  }

  if ($op == t('Login')) {
    btrClient_authenticate($form, $form_state);
    return;
  }

  if ($op == t('Save') and !btr_user_is_authenticated()) {
    btrClient_authenticate($form, $form_state);
    return;
  }

  if (btr_user_is_authenticated()) {
    // Save the values selected on the form (votes or new suggestions).
    _btrClient_translate_form_save($form_values);
  }

  if ($op == t('Save')) {
  }
  else {
    $lng = $form_values['langcode'];
    if ($op == t('Next')) {
      drupal_goto("translations/$lng/");
    }
    elseif ($op == t('Translated')) {
      drupal_goto("translations/$lng/translated");
    }
    elseif ($op == t('Untranslated')) {
      drupal_goto("translations/$lng/untranslated");
    }
  }
}

/**
 * Authenticate the user (redirect to login).
 */
function btrClient_authenticate($form, $form_state) {
  if ($form === NULL) {
    // We are in a redirect-after-login, but login has failed
    // or was cancelled. Clear the session variable so that
    // it does not keep redirecting.
    unset($_SESSION['btrClient']['translate_form']['state']);
    return;
  }

  $_SESSION['btrClient']['translate_form']['state'] = $form_state;

  if (_btrClient_installed_on_server()) {
    drupal_goto('user/login', array('query' => drupal_get_destination()));
  }
  else {
    oauth2_login();
  }
}

/**
 * Save the values selected on the form (votes or new suggestions).
 */
function _btrClient_translate_form_save($form_values) {
  // Get the langcode submitted with the form.
  $lng = $form_values['langcode'];

  // Get the voting mode.
  $voting_mode = variable_get('btrClient_voting_mode', 'single');

  // Iterate outer structure built in btrClient_translate_form().
  foreach ($form_values['strings'] as $sguid => $string) {

    if ($voting_mode == 'single') {
      _btrClient_translate_form_submit_single($sguid, $lng, $string);
    }

    // Iterate inner structure built in _btrClient_translate_form_item().
    // Form items have numeric $tguid values and other keys here.
    foreach ($string as $tguid => $translation) {
      if ($voting_mode == 'multiple') {
        _btrClient_translate_form_submit_multiple($sguid, $tguid, $lng, $translation);
      }
      if ((strlen($tguid) == 40) && !empty($translation['declined'])) {
        // Delete translation.
        btrClient_add_action('del', array('tguid' => $tguid));
      }
    }
  }

  // Submit the actions.
  global $_btrclient_actions;
  if (empty($_btrclient_actions)) {
    return;
  }
  $btr = wsclient_service_load('btr');
  $result = $btr->submit($_btrclient_actions);

  // Display any messages, warnings and errors.
  btrClient_display_messages($result['messages']);
}

/**
 * Add an action to the list of actions.
 */
function btrClient_add_action($action, $params) {
  global $_btrclient_actions;
  $_btrclient_actions[] = array('action' => $action, 'params' => $params);
}

/**
 * Return true if a new translation has been submitted.
 */
function is_not_empty_translation($translation) {
  $translation = btrClient_pack_string($translation);
  $translation = str_replace(t('<New translation>'), '', $translation);
  $translation = trim($translation);
  return !empty($translation);
}

/**
 * Form submit for the case of voting mode 'single'.
 */
function _btrClient_translate_form_submit_single($sguid, $lng, $string) {

  if (is_not_empty_translation($string['new']['value'])) {
    // Add a new suggestion.
    btrClient_add_action('add', array(
        'sguid' => $sguid,
        'lng' => $lng,
        'translation' => $string['new']['value'],
      ));
    return;
  }

  $tguid = $string['approved'];
  if (strlen($tguid) == 40) {
    // If this is not an existing vote,
    // then add a new vote for this translation.
    $previous_votes = $string[$tguid]['original']['votes'];
    global $user;
    if (!in_array($user->name, array_keys($previous_votes))) {
      btrClient_add_action('vote', array('tguid' => $tguid));
    }
  }
}

/**
 * Form submit for the case of voting mode 'multiple'.
 */
function _btrClient_translate_form_submit_multiple($sguid, $tguid, $lng, $translation) {

  global $user;

  $approved = $translation['approved'];
  if ($tguid == 'new' and is_not_empty_translation($translation['value'])) {
    btrClient_add_action('add', array(
        'sguid' => $sguid,
        'lng' => $lng,
        'translation' => $translation['value'],
      ));
  }
  elseif ($approved != '') {
    // Add a new vote for this translation
    // if such a vote does not exist.
    $previous_votes = $translation['original']['votes'];
    if (!in_array($user->name, array_keys($previous_votes))) {
      btrClient_add_action('vote', array('tguid' => $tguid));
    }
  }
  elseif ($approved == '') {
    // Remove this vote, if it exists.
    $previous_votes = $translation['original']['votes'];
    if (in_array($user->name, array_keys($previous_votes))) {
      btrClient_add_action('del_vote', array('tguid' => $tguid));
    }
  }
}
