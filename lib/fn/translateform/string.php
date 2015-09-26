<?php
/**
 * @file
 * Function: translateform_string()
 */

namespace BTranslator\Client;
use \bcl;

require_once __DIR__ . '/theme_functions.inc';

/**
 * Creates the form fragment for a source string and its translations.
 */
function translateform_string($string, $lng) {

  $string['string'] = bcl::string_unpack($string['string']);

  $form = array(
    '#string' => $string,
    '#langcode' => $lng,
    'source' => array(
      'string' => array('#markup' => _render($string['string'])),
    ),
  );

  $form['source']['edit'] = array(
    '#markup' => t('Translate'),
    '#prefix' => '<label title="' . t('Translate') . '">',
    '#suffix' => '</label>',
  );

  /*
  // // Add alternative strings (in other languages).
  // // TODO: Improve alternatives in other languages.
  // $arr_items = array();
  // foreach ($string['alternatives'] as $langcode => $string) {
  //   $string = bcl::string_unpack($string);
  //   $rendered_string = _render($string);
  //   $arr_items = array(
  //     '#markup' => "<strong>$langcode:</strong>"
  //                . "<label class='l10n-string'>$rendered_string</label>",
  //   );
  // }
  // $form['alternatives'] = array(
  //   '#theme' => 'item_list',
  //   '#title' => t('Alternatives in other languages:'),
  //   '#items' => $arr_items,
  //   '#prefix' => '<div class="l10n-usage">',
  //   '#suffix' => '</div>',
  // );
  */

  // ---------------- Add translations. --------------------

  // Translations are stored in an array.
  $translations = $string['translations'];

  foreach ($translations as $translation) {
    // Add the translation to the list.
    $tguid = $translation['tguid'];
    $form[$tguid] = _translation($translation, $string['sguid'], $lng);
  }

  // If the user may add new suggestions, display a textarea.
  $textarea = _new_translation($string, $lng);
  $form[$textarea['tguid']] = _translation($textarea, $string['sguid'], $lng);

  return $form;
}

/**
 * Build mock object for new textarea.
 */
function _new_translation($string, $lng) {
  // Fill in with as many items as required. If the source was plural, we
  // need to fill in with a number adequate for this language.
  $languages = bcl::get_languages();
  $nplurals = $languages[$lng]['plurals'];
  $arr_translations = array_fill(0, count($string['string']) > 1 ? $nplurals : 1, '');
  $translation = implode("\0", $arr_translations);

  return array(
    'sguid' => $string['sguid'],
    'tguid' => 'new',
    'lng' => $lng,
    'translation' => $translation,
    'count' => '0',
    'votes' => array(),
  );
}

/**
 * Creates the form fragment for a translated string.
 */
function _translation($translation, $string_sguid, $lng) {

  $translation['translation'] = bcl::string_unpack($translation['translation']);

  $btr_user = oauth2_user_get();
  $is_own = ($btr_user['name'] and ($btr_user['name'] == $translation['author']));
  $is_approved = ($btr_user['name'] and in_array($btr_user['name'], array_keys($translation['votes'])));
  $is_new = ($translation['tguid'] == 'new');
  $may_moderate = ($is_own or bcl::user_access('btranslator-resolve'));

  $form = array(
    '#theme' => 'btrClient_translate_translation',
    'original' => array(
      '#type' => 'value',
      '#value' => $translation,
    ),
  );

  // The 'approved' radio/checkbox is used to pick the approved (voted/liked)
  // translation(s).
  $voting_mode = variable_get('btrClient_voting_mode', 'single');
  $type = ($voting_mode == 'single') ? 'radio' : 'checkbox';
  $form['approved'] = array(
    '#type' => $type,
    '#theme' => 'btrClient_translate_radio',
    '#theme_wrappers' => array(),
    '#title' => _render($translation['translation'], $is_new ? t('(empty)') : FALSE),
    '#return_value' => $translation['tguid'],
    '#default_value' => $is_approved ? $translation['tguid'] : '',
    '#attributes' => array('class' => array('selector')),
  );
  if ($voting_mode == 'single') {
    $form['approved']['#checked'] = $is_approved;
    $form['approved']['#parents'] = array('strings', $string_sguid, 'approved');
  }

  if ($may_moderate && !$is_new) {
    $form['declined'] = array(
      '#type' => 'checkbox',
      '#title' => t('Decline'),
      '#default_value' => FALSE,
    );
  }

  if ($is_new) {
    // Fill in with as many textareas as required to enter translation
    // for this string.
    $form['value'] = array_fill(0, count($translation['translation']), array(
                       '#type' => 'textarea',
                       '#cols' => 60,
                       '#rows' => 3,
                       '#default_value' => t('<New translation>'),
		       '#attributes' => array(
                         'defaultValue' => t('<New translation>'),
                       ),
                     ));

  }
  else {
    $form['edit'] = array(
      '#markup' => t('Edit a copy'),
      '#prefix' => '<label title="' . t('Edit a copy') . '">',
      '#suffix' => '</label>',
    );
    if ($translation['author'] != NULL) {
      $form['author'] = array(
        '#markup' => _by($translation['author'], $translation['uid'], $translation['time']),
      );
    }
    // TODO: Improve displaying of vote count and the voters.
    if ($translation['count'] != '0') {
      $btr_server = variable_get('btrClient_server');
      $voters = array();
      foreach ($translation['votes'] as $name => $vote) {
        $voters[] = l($name, $btr_server . '/user/' . $vote['uid']);
      }
      $form['votes'] = array(
        '#type' => 'fieldset',
        '#title' => t('Votes: !count', array('!count' => $translation['count'])),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        'voters' => array(
          '#markup' => implode(', ', $voters),
        ),
      );
    }
  }

  return $form;
}


/**
 * Generate markup for an unpacked string.
 *
 * @param array $textarray
 *   An array of strings as generated by bcl::string_unpack().
 * @param string $empty
 *   Specific data to include as the data to use when empty.
 */
function _render($textarray, $empty = '') {
  // data-empty is a proprietary attribute used in editor.css to be displayed
  // when starting to submit a new suggestion.
  $empty = !empty($empty) ? ' data-empty="' . check_plain($empty) . '"' : '';
  return "<span$empty>" . implode("</span><br /><span$empty>", array_map('check_plain', $textarray)) . '</span>';
}


/**
 * Generates the 'By ...' line containing meta information about a string.
 */
function _by($name, $uid, $time) {
  $btr_server = variable_get('btrClient_server');
  $params = array(
    '!author' => l($name, $btr_server . "/user/$uid"),
    '@date' => format_date(strtotime($time)),
    '@timeago' => format_interval(REQUEST_TIME - strtotime($time)),
  );
  return t('by !author <span title="on @date">@timeago ago</span>', $params);
}
