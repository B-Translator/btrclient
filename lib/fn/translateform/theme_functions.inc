<?php

/**
 * @file
 * Theme functions.
 */

/**
 * Main theme function for translation table.
 */
function theme_btrClient_translate_table($variables) {

  $element = $variables['element'];
  if (sizeof(element_children($element)) == 1) {
    $output = _theme_btrClient_translate_table_1col($element);
  }
  else {
    $output = _theme_btrClient_translate_table_2col($element);
  }

  return $output;
}

/**
 * Format the output as 1-column table.
 */
function _theme_btrClient_translate_table_1col($element) {

  // Get the first element.
  foreach (element_children($element) as $key) { break; }
  $elem = $element[$key];

  return theme('table', [
      'attributes' => ['class' => ['l10n-table']],
      'rows' => [
        // row of the english string
        [
          [
            'class' => ['source'],
            'data' => theme('btrClient_translate_source', ['element' => $elem]),
          ],
        ],
        // row of the translations
        [
          [
            'class' => ['translation'],
            'data' => theme('btrClient_translate_translation_list', ['element' => $elem]),
          ],
        ],
      ],
    ]);
}

/**
 * Format the output as 2-column table.
 */
function _theme_btrClient_translate_table_2col($element) {

  $lng = $element['#lng'];
  $header = array(
    array(
      'data' => t('Source text'),
      'colspan' => 2,
    ),
    t('Translations') . " ($lng)",
  );

  $rows = array();

  foreach (element_children($element) as $key) {
    $lng = $element[$key]['#langcode'];
    $sguid = $element[$key]['#string']['sguid'];
    $rows[] = array(
      array(
        'class' => ['sguid'],
        'data' => l(t('#'), "translations/$lng/$sguid", [
                  'attributes' => [
                    'title' => t('Direct and permanent link to this string.'),
                    'class' => ['btn', 'btn-default'],
                  ]]),
      ),
      array(
        'class' => array('source'),
        'data' => theme('btrClient_translate_source', array('element' => $element[$key])),
      ),
      array(
        'class' => array('translation'),
        'data' => theme('btrClient_translate_translation_list', array('element' => $element[$key])),
      ),
    );
  }

  return theme('table', [
      'attributes' => ['class' => ['l10n-table']],
      'header' => $header,
      'rows' => $rows,
    ]);
}

/**
 * Theme function for a source cell in the translation table.
 */
function theme_btrClient_translate_source($variables) {
  $element = $variables['element'];
  $output = theme('btrClient_translate_actions', array('element' => $element['source']));
  $filter_match = ' filter-no-match';
  $output .= '<label class="l10n-string' . $filter_match . '">' . drupal_render($element['source']['string']) . '</label>';
  $output .= theme('btrClient_in_context', array('source' => $element['#string']));

  $lng = bcl::get_translation_lng();
  $sguid = $element['#string']['sguid'];
  $url = $GLOBALS['base_url'] . '/btr/ajax/string_details/' . $lng . '/' . $sguid;
  $output .= '<div class="l10n-usage"><a href="' . $url . '" class="l10n-more-link" title="' . t('Show the string context and a list of projects where this string is used.') . '">' . t('Show string details.') . '</a><div class="l10n-more-info"></div></div>';

  // TODO: Improve alternatives in other languages.
  // $output .= drupal_render($element['alternatives']);

  return $output;
}

/**
 * Theme appropriate actions for the given string element.
 */
function theme_btrClient_translate_actions($variables) {
  $actions = '';

  $votes = _theme_btrClient_translate_votes($variables['element']);
  if ($votes) {
    $actions .= '<li>' . $votes . '</li>';
  }

  $element = $variables['element'];
  foreach (['edit', 'declined'] as $type) {
    if (isset($element[$type])) {
      $actions .= '<li class="' . $type . '">' . drupal_render($element[$type]) . '</li>';
    }
  }

  if (!empty($actions)) {
    return '<ul class="actions">' . $actions . '</ul>';
  }
  else {
    return '';
  }
}

/**
 * Theme context information for source strings.
 */
function theme_btrClient_in_context($variables) {
  $string = $variables['source'];
  if (!empty($string->context)) {
    return '<div class="string-context">' . t('in context: @context', array('@context' => $string->context)) . '</div>';
  }
  return '';
}

/**
 * Theme a suggestion/translation.
 */
function theme_btrClient_translate_translation($variables) {
  $element = $variables['element'];

  $element['#attributes']['class'][] = 'clearfix';
  $element['#attributes']['class'][] = 'translation';

  // Add is-selectable and is-declinable helpers for JS.
  $element['#attributes']['class'][] = 'is-selectable';
  if (isset($element['declined'])) {
    $element['#attributes']['class'][] = 'is-declinable';
  }

  switch ($element['approved']['#return_value']) {
    case 'new':
      $element['#attributes']['class'][] = 'new-translation';
      break;

    case '0':
      $element['#attributes']['class'][] = 'no-translation';
      // Fallthrough.
    default:
      if ($element['approved']['#value'] !== '') {
        $element['#attributes']['class'][] = 'is-approved default';
      }
  }

  $output = '<li' . drupal_attributes($element['#attributes']) . '>';
  $output .= theme('btrClient_translate_actions', ['element' => $element]);
  // Add the radio box to pick the approved translation.
  $output .= drupal_render($element['approved']);

  if (isset($element['author'])) {
    $output .= _theme_btrClient_translate_author($element['author']);
  }
  if (isset($element['value'])) {
    $output .= drupal_render($element['value']);
  }

  return $output . '</li>';
}

/**
 * Theme a radio button to pick the approved translation.
 */
function theme_btrClient_translate_radio($variables) {
  $element = $variables['element'];
  $type = $element['#type'];
  _form_set_class($element, array('form-radio'));
  $output = '<input type="' . $type . '" ';
  $output .= 'id="' . $element['#id'] . '" ';
  $output .= 'name="' . $element['#name'] . '" ';
  $output .= 'value="' . $element['#return_value'] . '" ';
  $output .= $element['#checked'] ? ' checked="checked" ' : ' ';
  $output .= drupal_attributes($element['#attributes']) . ' />';

  if (isset($element['#title'])) {
    $output .= '<label class="l10n-string" for="' . $element['#id'] . '">' . $element['#title'] . '</label>';
  }
  return $output;
}

/**
 * Theme a list of suggestions for a string.
 */
function theme_btrClient_translate_translation_list($variables) {
  $element = $variables['element'];
  $output = '<ul>';
  foreach (element_children($element) as $child) {
    if ((strlen($child) == 40) || ($child == 'new')) {
      $output .= drupal_render($element[$child]);
    }
  }
  $output .= '</ul>';
  return $output;
}

function _theme_btrClient_translate_votes($element) {
  if (!isset($element['votes'])) return '';
  if ($element['approved']['#return_value'] == 'new')  return '';

  $count = $element['votes']['#count'];
  $voters = $element['votes']['#voters'];

  // Convert voters to an html list.
  $btr_server = variable_get('btrClient_server');
  $list = array();
  foreach ($voters as $name => $voter) {
    $url = $btr_server . '/user/' . $voter['uid'];
    $list[] = "<li><a href='$url' target='_blank'>$name</a></li>";
  }
  $content_html = '<ul>' . implode('', $list) . '</ul>';
  $content_html = strtr($content_html, "'", '"');

  $output = "<div class='votes'><span class='btn btn-info btn-sm' data-toggle='popover' data-trigger='click' data-placement='bottom auto' data-html='true' data-content='$content_html'>+ $count</span></div>";

  return $output;
}

/**
 * Display information about the author of the string.
 */
function _theme_btrClient_translate_author($author) {
  $time = format_interval(REQUEST_TIME - strtotime($author['#time']));
  if ($author['#name'] =='' or $author['#name'] == 'admin') {
    $msg = t('Added !time ago', ['!time' => $time]);
  }
  else {
    $btr_server = variable_get('btrClient_server');
    $name = l($author['#name'], $btr_server . '/user/' . $author['uid']);
    $msg = t('Added by !author !time ago', ['!author' => $name, '!time' => $time]);
  }
  return "<div class='author'>$msg</div>";
}
