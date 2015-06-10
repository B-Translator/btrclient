<?php
/**
 * @file
 * Menus.
 */

/**
 * Implements hook_menu().
 */
function btrClient_menu() {

  $items = array();

  $items['admin/config/bcl'] = array(
    'title' => 'B-Translator Client',
    'description' => 'B-Translator Client Profile',
    'weight' => -100,
    'page callback' => 'system_admin_menu_block_page',
    'file' => 'system.admin.inc',
    'file path' => drupal_get_path('module', 'system'),
    'access callback' => TRUE,
  );

  $items['admin/config/bcl/client'] = array(
    'title'            => 'BTR Client',
    'description'      => 'B-Translator Client configuration settings.',
    'page callback'    => 'drupal_get_form',
    'page arguments'   => array('btrClient_config'),
    'access callback'  => TRUE,
    'file'             => 'btrClient.admin.inc',
  );

  $items['translations'] = array(
    'type'            => MENU_NORMAL_ITEM,
    'title'           => 'Translations',
    'description'     => 'Vote the best translation for a string, or suggest a better translation.',
    'page callback'   => 'btrClient_vote',
    'file'            => 'includes/vote.inc',
    'access callback' => TRUE,
  );

  $items['translations/vote'] = array(
    'type'            => MENU_DEFAULT_LOCAL_TASK,
    'title'           => 'Vote',
    'weight'          => 1,
  );

  $items['translations/search'] = array(
    'type'            => MENU_LOCAL_TASK,
    'title'           => 'Search',
    'description'     => 'Search strings and translations using various filters.',
    'page callback'   => 'btrClient_search',
    'file'            => 'includes/search.inc',
    'access callback' => TRUE,
    'weight'          => 2,
  );

  $items['translations/export'] = array(
    'type'            => MENU_LOCAL_TASK,
    'title'           => 'Export',
    'description'     => 'Export the most voted translations of a project into a PO file.',
    'page callback'   => 'btrClient_export',
    'file'            => 'includes/export.inc',
    'access callback' => 'btr_user_is_authenticated',
    'weight'          => 3,
  );

  $items['translations/import'] = array(
    'type'            => MENU_LOCAL_TASK,
    'title'           => 'Import Translations',
    'description'     => 'Import translations from PO files.',
    'page callback'   => 'btrClient_import',
    'file'            => 'includes/import.inc',
    'access callback' => 'btr_user_access',
    'access arguments' => array('btranslator-import'),
    'weight'          => 4,
  );

  $items['translations/import_project'] = array(
    'type'            => MENU_LOCAL_TASK,
    'title'           => 'Import Project',
    'description'     => 'Import project from PO/POT file(s).',
    'page callback'   => 'btrClient_importproject',
    'file'            => 'includes/import.inc',
    'access callback' => 'btr_user_access',
    'access arguments' => array('btranslator-import'),
    'weight'          => 5,
  );

  // Return a RSS feed.
  $items['btr/rss-feed'] = array(
    'type'            => MENU_CALLBACK,
    'title'           => 'Latest Translation Feedback',
    'page callback'   => 'btrClient_rss_feed',
    'file'            => 'includes/rss_feed.inc',
    'access callback' => TRUE,
  );

  // Return a random tweet from B-Translator.
  $items['btr/tweet'] = array(
    'type'            => MENU_CALLBACK,
    'title'           => 'Return a random string (and translations) in a form suitable for twitter.',
    'page callback'   => 'btrCore_tweet',
    'file'            => 'includes/tweet.inc',
    'access callback' => TRUE,
  );

  return $items;
}

/**
 * Return true if there is an oauth2 access_token.
 */
function btr_user_is_authenticated() {
  return bcl::user_is_authenticated();
}

/**
 * Check whether the current user has the given permission (on the server).
 */
function btr_user_access($permission) {
  return bcl::user_access($permission);
}

/**
 * Implements hook_menu_alter().
 */
function btrClient_menu_alter(&$items) {
  // Move the configuration of oauth2_login to the section of B-Translator.
  $items['admin/config/bcl/oauth2_login'] = $items['admin/config/people/oauth2_login'];
  unset($items['admin/config/people/oauth2_login']);
}
