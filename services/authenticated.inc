<?php
/**
 * @file
 * Define authenticated web services.
 */

/**
 * Create and return the authenticated service.
 */
function _btrClientWS_authenticated($service_name) {
  $server_url = variable_get('oauth2_login_oauth2_server', '');

  $service = new WSClientServiceDescription();
  $service->name = $service_name;
  $service->label = 'Authenticated B-Translator Services';
  $service->type = 'rest';
  $service->url = $server_url . '/api/';

  $service->settings += oauth2_user_wsclient_dev_settings();
  $service->settings += oauth2_user_wsclient_auth_settings();

  $service->operations += _btrClientWS_authenticated_translations();
  $service->operations += _btrClientWS_authenticated_projects();

  return $service;
}

/**
 * Return definitions for operations about translations.
 */
function _btrClientWS_authenticated_translations() {
  $operations['get_translations'] = array(
    'label' => 'get_translations',
    'url' => 'translations/@sguid',
    'parameter' => array(
      'sguid' => array('type' => 'text'),
      'lng' => array('type' => 'text'),
    ),
  );

  $post_operations = array(
    'search' => 'translations/search',
    'get_random_sguid' => 'translations/get_random_sguid',
    'add_translation' => 'translations/add',
    'add_vote' => 'translations/vote',
    'del_translation' => 'translations/del',
    'del_vote' => 'translations/del_vote',
    'submit' => 'translations/submit',
    'import_translations' => 'translations/import',
    'delete_project' => 'project/delete',
  );
  foreach ($post_operations as $name => $url) {
    $operations[$name] = array(
      'label' => $name,
      'url' => $url,
      'type' => 'POST',
      'data' => 'params',
      'parameter' => array(
        'params' => array('type' => 'array'),
      ),
    );
  }

  return $operations;
}

/**
 * Return definitions for operations about projects.
 */
function _btrClientWS_authenticated_projects() {
  $post_operations = array(
    'subscribe' => 'project/subscribe',
    'unsubscribe' => 'project/unsubscribe',
    'subscriptions' => 'project/subscriptions',
    'export_projects' => 'project/export',
    'add_string' => 'project/add_string',
    'del_string' => 'project/del_string',
  );
  foreach ($post_operations as $name => $url) {
    $operations[$name] = array(
      'label' => $name,
      'url' => $url,
      'type' => 'POST',
      'data' => 'params',
      'parameter' => array(
        'params' => array('type' => 'array'),
      ),
    );
  }

  return $operations;
}
