<?php
/**
 * @file
 * The function btrClient_translate_form() builds a render array
 * for the strings and their corresponding translations/suggestions.
 * The edit and submit functionality will be available only when suitable.
 */

/**
 * Form callback: List strings and the corresponding translations/suggestions.
 *
 * @param array $form
 *   The form array.
 * @param array $form_state
 *   The form state array.
 * @param array $strings
 *   A multi-dimentional associative array of the string and the corresponding
 *   translations that are to be rendered.
 * @param string $lng
 *   The language code of the translations.
 *
 * @return array
 *   A render array of the page.
 */
function btrClient_translate_form($form, &$form_state, $strings, $lng) {
  $pager = theme('pager', array('tags' => NULL, 'element' => 0));
  $form = array(
    'langcode' => array(
      '#type' => 'value',
      '#value' => $lng,
    ),
    'pager_top' => array(
      '#weight' => -10,
      '#markup' => $pager,
    ),
    'strings' => array(
      '#tree' => TRUE,
      '#theme' => 'btrClient_translate_table',
      '#lng' => $lng,
    ),

    'buttons' => _btrClient_translate_form_buttons($lng),

    'pager_bottom' => array(
      '#weight' => 10,
      '#markup' => $pager,
    ),
  );

  // Fill in string values for the editor.
  foreach ($strings as $string) {
    $sguid = $string['sguid'];
    $form['strings'][$sguid] = _btrClient_translate_form_item($form_state, $string, $lng);
    // TODO: Display the number of comments for each string.
  }

  // If there is only one string.
  if (count($strings) == 1) {
    // Set the page title (no longer than 20 chars).
    $title = btrClient_shorten($string['string'], 20);
    drupal_set_title(t('String: !string', array('!string' => $title),
        array('context' => 'set the page title')));

    // Add metatags (og:title, og:description, og:image).
    _btrClient_translate_form_add_metatags($string);

    // Append the social share buttons and comments to the form.
    $path = "translations/$lng/$sguid";
    $title = btrClient_shorten($string['string'], 50);
    _btrClient_translate_form_add_share_and_comment($form, $path, $title);
  }

  return $form;
}

/**
 * Get the buttons of the form.
 */
function _btrClient_translate_form_buttons($lng) {
  // $buttons['#prefix'] = '<div class="filter-submit">';
  // $buttons['#suffix'] = '</div>';

  // Don't show the Back button on the search interface.
  if (current_path() != 'translations/search' && isset($_SESSION['btrClient']['previous_url'])) {
    $buttons['previous'] = array(
      '#type' => 'submit',
      '#value' => t('Previous'),
    );
  }

  $buttons['login'] = array(
    '#type' => 'submit',
    '#value' => t('Login'),
    '#access' => !btr_user_is_authenticated(),
  );

  // The submit buttons will appear only when the user has
  // permissions to submit votes and suggestions.
  $translation_lng = variable_get('btrClient_translation_lng', 'all');
  $enable_submit = ($translation_lng == 'all' or ($translation_lng == $lng));
  $buttons['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
    '#access' => $enable_submit,
  );

  // Don't show the Next buttons on the search interface.
  if (current_path() != 'translations/search') {
    $buttons['next'] = array(
      '#type' => 'submit',
      '#value' => t('Next'),
    );
    $buttons['translated'] = array(
      '#type' => 'submit',
      '#value' => t('Translated'),
    );
    $buttons['untranslated'] = array(
      '#type' => 'submit',
      '#value' => t('Untranslated'),
    );
    $buttons['same_project'] = array(
      '#type' => 'checkbox',
      '#title' => t('Same Project'),
      '#value' => isset($_SESSION['btrClient']['same_project']) ? $_SESSION['btrClient']['same_project'] : FALSE,
    );
  }

  return $buttons;
}

/**
 * Append the social share buttons and comments to the form.
 */
function _btrClient_translate_form_add_share_and_comment(&$form, $path, $title) {
  $url = url($path, array('absolute' => TRUE));

  if (module_exists('service_links')) {
    // Define the service_links form element.
    $style = SERVICE_LINKS_STYLE_IMAGE;
    $form['service_links'] = array(
      '#weight' => 100,
      '#prefix' => '<div id="service-links-block">',
      '#markup' => '<h3>' . t('Share:') . '</h3>'
      . theme('service_links_block_format', array(
          'items' => service_links_render(NULL, FALSE, $style),
          'style' => $style,
        )),
      '#suffix' => '</div>',
    );
  }

  if (module_exists('sharethis')) {
    // Define the sharethis form element.
    $data_options = sharethis_get_options_array();
    $form['sharethis'] = array(
      '#weight' => 100,
      '#prefix' => '<div id="sharethis-buttons">',
      '#markup' => theme('sharethis', array(
                   'data_options' => $data_options,
                   'm_path' => $url,
                   'm_title' => "String: $title",
                 )),
      '#suffix' => '</div>',
    );
  }

  if (module_exists('disqus')) {
    // Define the disqus form element.
    $form['disqus'] = array(
      '#type' => 'disqus',
      '#disqus' => array(
        'domain' => variable_get('disqus_domain', 'dev-btranslator'),
        'status' => TRUE,
        'url' => $url,
        'title' => $title,
        'identifier' => $path,
        'developer' => variable_get('disqus_developer', '1'),
      ),
      '#weight' => 101,
    );
  }
}

/**
 * Add metatags (og:title, og:description, og:image, og:image:size).
 */
function _btrClient_translate_form_add_metatags($string, $vocabulary = NULL) {
  // Get the title.
  $str = $string['string'];
  if ($vocabulary) {
    $str = t('Vocabulary') . " $vocabulary: " . $str;
  }
  else {
    $str = t('String') . ': ' . $str;
  }
  $title = btrClient_shorten($str, 100);

  // Get the description.
  $description = $string['string'];
  $arr_translations = array();
  foreach ($string['translations'] as $trans) {
    $arr_translations[] = strip_tags($trans['translation']);
  }
  if (!empty($arr_translations)) {
    $description .= ' <==> ' . implode(' / ', $arr_translations);
  }

  // Get the url.
  if (isset($_GET['url'])) {
    $url = $_GET['url'];
    $description .= " (see: $url)";
  }
  else {
    $url = $GLOBALS['base_root'] . request_uri();
  }

  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      "property" => "og:title",
      "content" => $title,
    ),
  );
  drupal_add_html_head($element, 'og_title');

  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      "property" => "og:description",
      "content" => $description,
    ),
  );
  drupal_add_html_head($element, 'og_description');

  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      "property" => "og:url",
      "content" => $url,
    ),
  );
  drupal_add_html_head($element, 'og_url');

  // Add og:image
  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      "property" => "og:image",
      "content" => $GLOBALS['base_url'] . '/logo.png',
    ),
  );
  drupal_add_html_head($element, 'og_image');

  // Add og:image:width
  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      "property" => "og:image:width",
      "content" => '24',
    ),
  );
  drupal_add_html_head($element, 'og_image_width');

  // twitter:card
  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      "property" => "twitter:card",
      "content" => 'Summary',
    ),
  );
  drupal_add_html_head($element, 'twitter_card');

  // twitter:site
  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      "property" => "twitter:site",
      "content" => '@l10n_sq',
    ),
  );
  drupal_add_html_head($element, 'twitter_site');

  // twitter:title
  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      "property" => "twitter:title",
      "content" => $title,
    ),
  );
  drupal_add_html_head($element, 'twitter_title');

  // twitter:description
  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      "property" => "twitter:description",
      "content" => $description,
    ),
  );
  drupal_add_html_head($element, 'twitter_description');
}

/**
 * Creates the form fragment for a source string and its translations.
 */
function _btrClient_translate_form_item(&$form_state, $string, $lng) {

  $string['string'] = btrClient_unpack_string($string['string']);

  $form = array(
    '#string' => $string,
    '#langcode' => $lng,
    'source' => array(
      'string' => array('#markup' => _btrClient_translate_render_textarray($string['string'])),
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
  //   $string = btrClient_unpack_string($string);
  //   $rendered_string = _btrClient_translate_render_textarray($string);
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
    $form[$tguid] = _btrClient_translate_translation($translation, $string['sguid'], $lng);
  }

  // If the user may add new suggestions, display a textarea.
  $textarea = _btrClient_translate_translation_textarea($string, $lng);
  $form[$textarea['tguid']] = _btrClient_translate_translation($textarea, $string['sguid'], $lng);

  return $form;
}

/**
 * Build mock object for new textarea.
 */
function _btrClient_translate_translation_textarea($string, $lng) {
  global $user;

  // Fill in with as many items as required. If the source was plural, we
  // need to fill in with a number adequate for this language.
  $languages = btrClient_get_languages();
  $nplurals = $languages[$lng]['plurals'];
  $arr_translations = array_fill(0, count($string['string']) > 1 ? $nplurals : 1, '');
  $translation = implode("\0", $arr_translations);

  return array(
    'sguid' => $string['sguid'],
    'tguid' => 'new',
    'lng' => $lng,
    'translation' => $translation,
    'author' => NULL,
    'uid' => $user->uid,
    'count' => '0',
    'votes' => array(),
  );
}

/**
 * Creates the form fragment for a translated string.
 */
function _btrClient_translate_translation($translation, $string_sguid, $lng) {

  global $user;

  $translation['translation'] = btrClient_unpack_string($translation['translation']);

  $is_own = ($user->uid && ($user->name == $translation['author']));
  $is_approved = ($user->uid && in_array($user->name, array_keys($translation['votes'])));
  $is_new = ($translation['tguid'] == 'new');
  $may_moderate = ($is_own or btr_user_access('btranslator-resolve'));

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
    '#title' => _btrClient_translate_render_textarray($translation['translation'], $is_new ? t('(empty)') : FALSE),
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
        '#markup' => _btrClient_translate_byline($translation['author'], $translation['uid'], $translation['time']),
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
 *   An array of strings as generated by btrClient_unpack_string().
 * @param string $empty
 *   Specific data to include as the data to use when empty.
 */
function _btrClient_translate_render_textarray($textarray, $empty = '') {
  // data-empty is a proprietary attribute used in editor.css to be displayed
  // when starting to submit a new suggestion.
  $empty = !empty($empty) ? ' data-empty="' . check_plain($empty) . '"' : '';
  return "<span$empty>" . implode("</span><br /><span$empty>", array_map('check_plain', $textarray)) . '</span>';
}


/**
 * Generates the byline containing meta information about a string.
 */
function _btrClient_translate_byline($name, $uid, $time) {
  $btr_server = variable_get('btrClient_server');
  $params = array(
    '!author' => l($name, $btr_server . "/user/$uid"),
    '@date' => format_date(strtotime($time)),
    '@timeago' => format_interval(REQUEST_TIME - strtotime($time)),
  );
  return t('by !author <span title="on @date">@timeago ago</span>', $params);
}
