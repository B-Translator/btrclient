<?php
/**
 * @file
 * Function: translateform_save()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Save the values selected on the form (votes or new suggestions).
 */
function translateform_save($form_values) {
  // Get the langcode submitted with the form.
  $lng = $form_values['langcode'];

  // Get the voting mode.
  $voting_mode = variable_get('btrClient_voting_mode', 'single');

  // Iterate outer structure built in bcl::translateform_build().
  foreach ($form_values['strings'] as $sguid => $string) {

    if ($voting_mode == 'single') {
      _submit_single($sguid, $lng, $string);
    }

    // Iterate the inner structure built in bcl::translateform_string().
    // Form items have numeric $tguid values and other keys here.
    foreach ($string as $tguid => $translation) {
      if ($voting_mode == 'multiple') {
        _submit_multiple($sguid, $tguid, $lng, $translation);
      }
      if ((strlen($tguid) == 40) && !empty($translation['declined'])) {
        // Delete translation.
        _add_action('del', array('tguid' => $tguid));
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
  bcl::display_messages($result['messages']);
}

/**
 * Add an action to the list of actions.
 */
function _add_action($action, $params) {
  global $_btrclient_actions;
  $_btrclient_actions[] = array('action' => $action, 'params' => $params);
}

/**
 * Return true if a new translation has been submitted.
 */
function _not_empty_translation($translation) {
  $translation = bcl::string_pack($translation);
  $translation = str_replace(t('<New translation>'), '', $translation);
  $translation = trim($translation);
  return !empty($translation);
}

/**
 * Form submit for the case of voting mode 'single'.
 */
function _submit_single($sguid, $lng, $string) {

  if (_not_empty_translation($string['new']['value'])) {
    // Add a new suggestion.
    _add_action('add', array(
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
    $btr_user = oauth2_user_get();
    if (!in_array($btr_user['name'], array_keys($previous_votes))) {
      _add_action('vote', array('tguid' => $tguid));
    }
  }
}

/**
 * Form submit for the case of voting mode 'multiple'.
 */
function _submit_multiple($sguid, $tguid, $lng, $translation) {

  $btr_user = oauth2_user_get();

  $approved = $translation['approved'];
  if ($tguid == 'new' and _not_empty_translation($translation['value'])) {
    _add_action('add', array(
        'sguid' => $sguid,
        'lng' => $lng,
        'translation' => $translation['value'],
      ));
  }
  elseif ($approved != '') {
    // Add a new vote for this translation
    // if such a vote does not exist.
    $previous_votes = $translation['original']['votes'];
    if (!in_array($btr_user['name'], array_keys($previous_votes))) {
      _add_action('vote', array('tguid' => $tguid));
    }
  }
  elseif ($approved == '') {
    // Remove this vote, if it exists.
    $previous_votes = $translation['original']['votes'];
    if (in_array($btr_user['name'], array_keys($previous_votes))) {
      _add_action('del_vote', array('tguid' => $tguid));
    }
  }
}
