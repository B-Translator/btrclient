<?php
/**
 * @file
 * Function: filter_form()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Build and return the filter form.
 */
function filter_form($form_values) {
  $form = [
    '#prefix' => '<div id="filter-form">',
    '#suffix' => '</div>',

    /* css attachment does not work
    '#attached' => [
      'css' => [
        drupal_get_path('module', 'btrClient') . '/lib/fn/filter/style.css',
      ],
    ],
    */

    'basic' => [
      '#type' => 'fieldset',

      // buttons
      'submit' => [
        '#value' => '<span class="glyphicon glyphicon-repeat"></span>',
        '#type' => 'submit',
        '#attributes' => ['class' => ['btn-primary']],
      ],

      // advanced search checkbox
      'options' => [
        '#markup' => '
            <div class="btn-group" data-toggle="buttons" style="float: right;">
              <label class="btn btn-default">
                <input type="checkbox" id="edit-options" name="options">
                <span class="glyphicon glyphicon-option-vertical"></span>
              </label>
            </div>
        ',
      ],

      // words to be searched
      'words' => [
        '#type' => 'textfield',
        '#default_value' => $form_values['words'],
      ],
    ],

    // advanced search fieldset
    'advanced' => [
      '#type' => 'fieldset',
      '#states' => [
        'visible' => [
          ':input[name="options"]' => ['checked' => TRUE],
        ]],
    ],
  ];

  // Add form fieldset advanced.
  $form['advanced'] += _advanced($form_values);

  return $form;
}

/**
 * Return the fieldset of advanced search.
 */
function _advanced($form_values) {
  // Language of translations.
  $languages = bcl::get_languages();
  foreach ($languages as $code => $lang) {
    $lang_options[$code] = $lang['name'];
  }

  // The number of results that should be displayed per page.
  list($limit_options, $default) = bcl::filter_get_options('limit', 'assoc');

  $advanced = [
    // Language of translations.
    'lng' => [
      '#type' => 'select',
      '#title' => t('Language of Translation'),
      '#description' => t('Select the language of the translations to be searched and displayed.'),
      '#options' => $lang_options,
      '#default_value' => $form_values['lng'],
    ],

    // Number of results that can be displayed on a page.
    'limit' => [
      '#type' => 'select',
      '#title' => t('Limit'),
      '#description' => t('The number of the results (strings) that can be displayed on a page.'),
      '#options' => $limit_options,
      '#default_value' => $form_values['limit'],
    ],

    // search mode
    'search_mode' => _search_mode($form_values),

    // projects
    'projects' => _projects($form_values),
  ];

  // Search by author and by date are available only to authenticated users.
  if (oauth2_user_is_authenticated()) {
    $advanced['author'] = _author($form_values);
    $advanced['date'] = _date($form_values);
  }

  return $advanced;
}

/**
 * Return search mode.
 */
function _search_mode($form_values) {
  list($search_mode_options, $default) = bcl::filter_get_options('mode', 'assoc');

  $params = array(
    '!url1' => 'http://dev.mysql.com/doc/refman/5.1/en/fulltext-natural-language.html',
    '!url2' => 'http://dev.mysql.com/doc/refman/5.1/en/fulltext-boolean.html',
  );
  $description = t('Search for l10n strings or translations that contain the given words. The <emphasized>natural</emphasized> search will try to find strings similar to the given one (see: <a href="!url1">Natural Language Full-Text Searches</a>). The <emphasized>boolean</emphasized> search will try to match words according to logical rules. The words can be preceded by + (plus), - (minus), etc. (for more details see: <a href="!url2">Boolean Full-Text Searches</a>).', $params);

  $search_mode = [
    '#type' => 'fieldset',
    '#title' => t('Search Mode'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,

    // mode
    'mode' => [
      '#type' => 'select',
      '#title' => t('Search Mode'),
      '#options' => $search_mode_options,
      '#default_value' => $form_values['mode'],
      '#description' => $description,
    ],
  ];

  return $search_mode;
}

/**
 * Return project/origin fields.
 */
function _projects($form_values) {
  $btr_server = variable_get('btrClient_server');

  $projects = [
    '#type' => 'fieldset',
    '#title' => t('Projects'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,

    // project
    'project' => [
      '#type' => 'textfield',
      '#title' => t('Project'),
      '#description' => t('Search only the strings belonging to the matching project.'),
      '#default_value' => $form_values['project'],
      '#autocomplete_path' => $btr_server . '/auto/project',
    ],

    // origin
    'origin' => [
      '#type' => 'textfield',
      '#title' => t('Origin'),
      '#description' => t('Limit search only to the projects from a certain origin.'),
      '#default_value' => $form_values['origin'],
      '#autocomplete_path' => $btr_server . '/auto/origin',
    ],
  ];

  return $projects;
}

/**
 * Return the author fieldset.
 */
function _author($form_values) {
  $btr_server = variable_get('btrClient_server');
  $lng = bcl::get_translation_lng();

  $author = [
    '#type' => 'fieldset',
    '#title' => t('Author'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,

    // Only mine.
    'only_mine' => [
      '#type' => 'checkbox',
      '#title' => t('Only Mine'),
      '#description' => t('Search only the strings with translations suggested or voted by me.'),
      '#default_value' => $form_values['only_mine'],
    ],

    // Translated by.
    'translated_by' => [
      '#type' => 'textfield',
      '#title' => t('Translated By'),
      '#description' => t('Search only the strings with translations suggested by the selected user.'),
      '#default_value' => $form_values['translated_by'],
      '#autocomplete_path' => $btr_server . '/auto/user/' . $lng,
      '#states' => [
        'visible' => [
          ':input[name="only_mine"]' => ['checked' => FALSE],
        ]],
    ],

    // Voted by.
    'voted_by' => [
      '#type' => 'textfield',
      '#title' => t('Voted By'),
      '#description' => t('Search only the strings with translations voted by the selected user.'),
      '#default_value' => $form_values['voted_by'],
      '#autocomplete_path' => $btr_server . '/auto/user/' . $lng,
      '#states' => [
        'visible' => [
          ':input[name="only_mine"]' => ['checked' => FALSE],
        ]],
    ],
  ];

  return $author;
}

/**
 * Return the date fieldset.
 */
function _date($form_values) {
  list($date_filter_options, $default) = bcl::filter_get_options('date_filter', 'assoc');

  $date = [
    '#type' => 'fieldset',
    '#title' => t('Date'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,

    // What to filter.
    'date_filter' => [
      '#type' => 'select',
      '#title' => t('What to Filter'),
      '#description' => t('Select what to filter by date (strings, translations, or votes).'),
      '#options' => $date_filter_options,
      '#default_value' => $form_values['date_filter'],
    ],

    // From date.
    'from_date' => [
      '#type' => 'textfield',
      '#title' => t('From Date'),
      '#default_value' => $form_values['from_date'],
    ],

    // To date.
    'to_date' => [
      '#type' => 'textfield',
      '#title' => t('To Date'),
      '#default_value' => $form_values['to_date'],
    ],
  ];

  return $date;
}
