<?php
/**
 * @file
 * Callback for menu item: 'btr/project/%/%/%/list'
 */

/**
 * Menu callback for 'btr/project/%/%/%/list'
 *
 * List strings and translations of the project.
 *
 * If the request header 'Accept: application/json' is set, then
 * the output will be in JSON, otherwise it will be an HTML page.
 */
function btrProject_list($origin, $project, $lng) {
  // Set the filter parameters.
  $params = bcl::filter_get_params();
  unset($params['sguid']);
  $params['origin'] = $origin;
  $params['project'] = $project;
  $params['lng'] = $lng;

  // Get the strings that match the search params.
  $btr = bcl::user_is_authenticated() ?
    wsclient_service_load('btr') :
    wsclient_service_load('public_btr');
  $result = $btr->search($params);

  // If the request has the header 'Accept: application/json'
  // or '?format=json' return the output in JSON format and stop.
  bcl::return_json($result['strings']);

  // Set the title.
  drupal_set_title(t('Strings of !origin/!project/!lng',
    ['!origin' => $origin, '!project' => $project, '!lng' => $lng]));

  // Add the filter form.
  $output[] = drupal_get_form('btrProject_listfilter_form', $result['filter']);

  // Output the strings.
  $strings = $result['strings'];
  if (empty($strings)) {
    drupal_set_message(t('No strings found. Try adjusting the filter options.'));
  }
  else {
    $pager = $result['pager'];
    pager_default_initialize($pager['number_of_items'], $pager['items_per_page']);
    $output[] = drupal_get_form('btrProject_list_form', $strings, $lng);

  }

  return $output;
}

/**
 * Form callback for: btrProject_list_form
 */
function btrProject_list_form($form, &$form_state, $strings, $lng) {
  $form = bcl::translateform_build($strings, $lng);
  return $form;
}

/**
 * Submit callback for the form: btrProject_list_form
 */
function btrProject_list_form_submit($form, &$form_state) {
  bcl::translateform_submit($form, $form_state);
}

/**
 * Form callback for: btrProject_listfilter_form
 */
function btrProject_listfilter_form($form, &$form_state, $filter) {
  // If the form has no values yet (either it is the first time that
  // the form is opened, or we have a get request), we set the values
  // of the form according to the $filter values.
  if (empty($form_state['values'])) {
    $form_state['values'] = $filter;
  }
  $form_values = $form_state['values'];

  $form_values['show'] = isset($_GET['show']) ? $_GET['show'] : 'all';

  // The number of results that should be displayed per page.
  list($limit_options, $default) = bcl::filter_get_options('limit', 'assoc');

  $form = [
    '#prefix' => '<div id="listfilter-form">',
    '#suffix' => '</div>',

    // Filter strings.
    'list_mode' => [
      '#type' => 'select',
      '#title' => t('List:'),
      '#description' => t('What to list.'),
      '#options' => array(
        'all' => '',
        'translated' => t('Translated'),
        'untranslated' => t('Untranslated'),
      ),
      '#default_value' => $form_values['list_mode'],
      '#attributes' => ['title' => t('What to list.')],
    ],

    // Number of results that can be displayed on a page.
    'limit' => [
      '#type' => 'select',
      '#title' => t('Limit:'),
      '#description' => t('Number of results that can be displayed on a page.'),
      '#options' => $limit_options,
      '#default_value' => $form_values['limit'],
      '#attributes' => ['title' => t('Number of results that can be displayed on a page.')],
    ],

    // buttons
    'submit' => [
      '#value' => '<i class="fa fa-refresh"></i>', //t('Go'),
      '#type' => 'submit',
      '#attributes' => ['class' => ['btn-primary']],
    ],
  ];

  return $form;
}

/**
 * Submit callback for the form: btrProject_listfilter_form
 */
function btrProject_listfilter_form_submit($form, &$form_state) {
  module_load_include('php', 'btrClient', 'lib/fn/filter/submit');
  \BTranslator\Client\filter_submit($form, $form_state);
}
