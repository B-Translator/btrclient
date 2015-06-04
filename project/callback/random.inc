<?php
/**
 * @file
 * Callback for menu item: 'btr/project/%/%/%/random'
 */

/**
 * Include utility and theme functions.
 */
module_load_include('inc', 'btrClient', 'includes/utils');
module_load_include('inc', 'btrClient', 'includes/translation_list/translation_list');

/**
 * Menu callback for 'btr/project/%/%/%/random'
 *
 * It can also be 'btr/project/%/%/%/random/translated'
 * or 'btr/project/%/%/%/random/untranslated'
 * Get a random string from the project and return the
 * review page for it.
 */
function btrProject_random($origin, $project, $lng, $mode = 'next') {
  if (!in_array($mode, ['next', 'translated', 'untranslated'])) {
    $mode = 'next';
  }

  // If there is a $_SESSION['btrProject']['translate_form']['state'],
  // then this is a redirect after login. Call the submit function again.
  if (isset($_SESSION['btrProject']['translate_form']['state'])) {
    $form_state = $_SESSION['btrProject']['translate_form']['state'];
    unset($_SESSION['btrProject']['translate_form']['state']);
    // drupal_form_submit('btrProject_translate_form', $form_state); // ?
    btrProject_translate_form_submit(NULL, $form_state);
  }

  // Get a random sguid from this project.
  $btr = wsclient_service_load('public_btr');
  $result = $btr->get_random_sguid(
    array(
      'target' => $mode,
      'lng' => $lng,
      'scope' => "$origin/$project",
    ));
  btrClient_display_messages($result['messages']);
  $sguid = $result['sguid'];
  if (!$sguid) {
    drupal_set_message(t('No string found!'), 'warning');
    $mode == 'next' ?
      drupal_goto('btr/projects') :
      drupal_goto("btr/project/$origin/$project/$lng");
  }

  // Get the string and its translations.
  $result = $btr->get_translations($sguid, $lng);
  btrClient_display_messages($result['messages']);

  // If the request has the header 'Accept: application/json'
  // return the output in JSON format and stop.
  if ($_SERVER['HTTP_ACCEPT'] == 'application/json') {
    drupal_add_http_header('Content-Type', 'application/json; utf-8');
    print json_encode($result);
    exit;
  }

  // Go to another path if there is no string.
  $string = $result['string'];
  if (empty($string)) {
    drupal_set_message(t('No string found!'), 'warning');
    drupal_goto('btr/projects');
    return;
  }

  // Set the title.
  drupal_set_title(t('Random String From Project: !project',
      array('!project' => "$origin/$project/$lng")));

  // Add the CSS and JS files.
  drupal_add_css(drupal_get_path('module', 'btrClient') . '/editor/editor.css');
  drupal_add_js(drupal_get_path('module', 'btrClient') . '/editor/jquery.worddiff.js');
  drupal_add_js(drupal_get_path('module', 'btrClient') . '/editor/editor.js');

  // Add RTL style if the current language's direction is RTL.
  $languages = btrClient_get_languages();
  if ($languages[$lng]['direction'] == LANGUAGE_RTL) {
    drupal_add_css(drupal_get_path('module', 'btrClient') . '/editor-rtl.css');
  }

  // Add the stylesheet of btrProject.
  $css_file = drupal_get_path('module', 'btrProject');
  $css_file .= (btrProject_inside_iframe() ?
               '/btrProject.iframe.css' :
               '/btrProject.css');
  drupal_add_css($css_file);

  // Build and return the output.
  $output = drupal_get_form('btrProject_form', $lng, $sguid, $string);

  return $output;
}

/**
 * Return true if the project is being displayed inside an iframe.
 */
function btrProject_inside_iframe() {
  return (isset($_GET['display']) and $_GET['display'] == 'iframe');
}

/**
 * Form callback.
 */
function btrProject_form($form, &$form_state, $lng, $sguid, $string) {
  $output = drupal_get_form('btrClient_translate_form',
            [$sguid => $string], $lng,
            btrProject_form_buttons($lng, $sguid));
  return $output;
}

/**
 * Get the buttons of the form.
 */
function btrProject_form_buttons($lng, $sguid) {

  $buttons = array();
  // $buttons['#prefix'] = '<div class="filter-submit">';
  // $buttons['#suffix'] = '</div>';

  /*
  $buttons['login'] = array(
    '#type' => 'submit',
    '#value' => t('Login'),
    '#access' => !btr_user_is_authenticated(),
  );
  */

  if ($sguid) {
    $attributes = array(
      'class' => 'btn form-submit',
      'style' => 'font-weight:bold',
    );
    if (btrProject_inside_iframe()) {
      $attributes['target'] = '_blank';
    }
    $buttons['details'] = array(
      '#markup' => l('#', "translations/$lng/$sguid",
                 array('attributes' => $attributes)),
    );

    // The save button will appear only when the user has
    // permissions to submit votes and suggestions.
    $translation_lng = variable_get('btrClient_translation_lng', 'all');
    $enable_save = ($translation_lng == 'all' or ($translation_lng == $lng));
    $buttons['save'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#access' => $enable_save,
      '#attributes' => array(
        'onclick' => 'this.form.target="_self"',
      ),
    );
    // When the user is not authenticated, clicking Save will redirect
    // to login. When we are inside an iFrame, it is better to do the
    // login on a popup window (or a new tab).
    if (!btr_user_is_authenticated() and btrProject_inside_iframe()) {
      $buttons['save']['#attributes']['onclick'] = 'this.form.target="_blank"';
    }
  }

  // Don't show the Next buttons on the search interface.
  $args = explode('/', current_path());
  if ($args[5] != 'search') {
    $buttons['next'] = array(
      '#type' => 'submit',
      '#value' => t('Next'),
      '#attributes' => array(
        'onclick' => 'this.form.target="_self"',
      ),
    );
    $buttons['translated'] = array(
      '#type' => 'submit',
      '#value' => t('Translated'),
      '#attributes' => array(
        'onclick' => 'this.form.target="_self"',
      ),
    );
    $buttons['untranslated'] = array(
      '#type' => 'submit',
      '#value' => t('Untranslated'),
      '#attributes' => array(
        'onclick' => 'this.form.target="_self"',
      ),
    );
  }

  return $buttons;
}
