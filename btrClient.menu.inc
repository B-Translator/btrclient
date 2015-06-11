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
 * Implements hook_menu_alter().
 */
function btrClient_menu_alter(&$items) {
  // Move the configuration of oauth2_login to the section of B-Translator.
  $items['admin/config/bcl/oauth2_login'] = $items['admin/config/people/oauth2_login'];
  unset($items['admin/config/people/oauth2_login']);
}
