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
  $form = array(
    // '#prefix' => '<div class="bcl-ui-filter clear-block">',
    '#prefix' => '<div class="clear-block">',
    '#suffix' => '</div>',
  );

  // Add form fields: lng, mode, words, limit.
  _basic_search($form, $form_values);

  // Add form fieldset advanced_search/projects, with the fields:
  // project and origin.
  _advanced_search($form, $form_values);

  // Search by author and serach by date
  // are available only to logged in users.
  if (bcl::user_is_authenticated()) {
    // Add fieldset advanced_search/author,
    // with fields: only_mine, translated_by, voted_by.
    _author($form, $form_values);

    // Add fieldset advanced_search/date,
    // with fields: date_filter, from_date, to_date.
    _date($form, $form_values);
  }

  $form['buttons'] = array(
    // '#prefix' => '<div class="filter-submit">',
    // '#suffix' => '</div>',
  );
  $form['buttons']['submit'] = array(
    '#value' => t('Filter'),
    '#type' => 'submit',
  );
  $form['buttons']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
  );

  // Enclose the whole form in another fieldset/container.
  $form1['search_container'] = array(
    '#type' => 'fieldset',
    '#title' => t('Change Filter Parameters'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  ) + $form;
  $form = $form1;

  return $form;
}

/**
 * Add form fields: lng, mode, words, limit.
 */
function _basic_search(&$form, $form_values) {

  $languages = bcl::get_languages();
  foreach ($languages as $code => $lang) {
    $lang_options[$code] = $lang['name'];
  }
  $form['lng'] = array(
    '#type' => 'select',
    '#title' => t('Language of Translation'),
    '#description' => t('Select the language of the translations to be searched and displayed.'),
    '#options' => $lang_options,
    '#default_value' => $form_values['lng'],
  );

  $form['basic_search'] = array(
    '#type' => 'fieldset',
    '#title' => t('What To Search For'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  list($search_mode_options, $default) = bcl::filter_get_options('mode', 'assoc');
  $form['basic_search']['mode'] = array(
    '#type' => 'select',
    '#title' => t('Search Mode'),
    '#options' => $search_mode_options,
    '#default_value' => $form_values['mode'],
  );

  $form['basic_search']['words'] = array(
    '#type' => 'textarea',
    '#title' => t('The String Or Words To Be Searched'),
    '#description' => t('Search for l10n strings or translations that contain the given words. The <emphasized>natural</emphasized> search will try to find strings similar to the given one (see: <a href="!url1">Natural Language Full-Text Searches</a>). The <emphasized>boolean</emphasized> search will try to match words according to logical rules. The words can be preceded by + (plus), - (minus), etc. (for more details see: <a href="!url2">Boolean Full-Text Searches</a>).',
                    array(
                      '!url1' => 'http://dev.mysql.com/doc/refman/5.1/en/fulltext-natural-language.html',
                      '!url2' => 'http://dev.mysql.com/doc/refman/5.1/en/fulltext-boolean.html',
                    )),
    '#default_value' => $form_values['words'],
    '#rows' => 2,
  );

  list($limit_options, $default) = bcl::filter_get_options('limit', 'assoc');
  $form['limit'] = array(
    '#type' => 'select',
    '#title' => t('Limit'),
    '#description' => t('The number of the results (strings) that can be displayed on a page.'),
    '#options' => $limit_options,
    '#default_value' => $form_values['limit'],
  );
}

/**
 * Add fieldset advanced_search/projects, with the fields: project and origin.
 */
function _advanced_search(&$form, $form_values) {

  $form['advanced_search'] = array(
    '#type' => 'fieldset',
    '#title' => t('Search Scope'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['advanced_search']['projects'] = array(
    '#type' => 'fieldset',
    '#title' => t('Projects'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $btr_server = variable_get('btrClient_server');
  $form['advanced_search']['projects']['project'] = array(
    '#type' => 'textfield',
    '#title' => t('Project'),
    '#description' => t('Search only the strings belonging to the matching project.'),
    '#default_value' => $form_values['project'],
    '#autocomplete_path' => $btr_server . '/auto/project',
  );

  $form['advanced_search']['projects']['origin'] = array(
    '#type' => 'textfield',
    '#title' => t('Origin'),
    '#description' => t('Limit search only to the projects from a certain origin.'),
    '#default_value' => $form_values['origin'],
    '#autocomplete_path' => $btr_server . '/auto/origin',
  );
}

/**
 * Add fieldset advanced_search/author.
 *
 * With fields: only_mine, translated_by, voted_by.
 */
function _author(&$form, $form_values) {

  $form['advanced_search']['author'] = array(
    '#type' => 'fieldset',
    '#title' => t('Author'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['advanced_search']['author']['only_mine'] = array(
    '#type' => 'checkbox',
    '#title' => t('Only Mine'),
    '#description' => t('Search only the strings with translations suggested or voted by me.'),
    '#default_value' => $form_values['only_mine'],
  );

  $btr_server = variable_get('btrClient_server');
  $lng = bcl::get_translation_lng();
  $form['advanced_search']['author']['translated_by'] = array(
    '#type' => 'textfield',
    '#title' => t('Translated By'),
    '#description' => t('Search only the strings with translations suggested by the selected user.'),
    '#default_value' => $form_values['translated_by'],
    '#autocomplete_path' => $btr_server . '/auto/user/' . $lng,
    '#states' => array(
      'visible' => array(
        ':input[name="only_mine"]' => array('checked' => FALSE),
      ),
    ),
  );

  $form['advanced_search']['author']['voted_by'] = array(
    '#type' => 'textfield',
    '#title' => t('Voted By'),
    '#description' => t('Search only the strings with translations voted by the selected user.'),
    '#default_value' => $form_values['voted_by'],
    '#autocomplete_path' => $btr_server . '/auto/user/' . $lng,
    '#states' => array(
      'visible' => array(
        ':input[name="only_mine"]' => array('checked' => FALSE),
      ),
    ),
  );
}

/**
 * Add fieldset advanced_search/date.
 *
 * With fields: date_filter, from_date, to_date.
 */
function _date(&$form, $form_values) {
  $form['advanced_search']['date'] = array(
    '#type' => 'fieldset',
    '#title' => t('Date'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  list($date_filter_options, $default) = bcl::filter_get_options('date_filter', 'assoc');
  $form['advanced_search']['date']['date_filter'] = array(
    '#type' => 'select',
    '#title' => t('What to Filter'),
    '#description' => t('Select what to filter by date (strings, translations, or votes).'),
    '#options' => $date_filter_options,
    '#default_value' => $form_values['date_filter'],
  );

  $form['advanced_search']['date']['from_date'] = array(
    '#type' => 'textfield',
    '#title' => t('From Date'),
    '#default_value' => $form_values['from_date'],
  );

  $form['advanced_search']['date']['to_date'] = array(
    '#type' => 'textfield',
    '#title' => t('To Date'),
    '#default_value' => $form_values['to_date'],
  );
}
