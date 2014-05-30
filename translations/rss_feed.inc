<?php
/**
 * @file
 * Menu callbacks.
 */

function btrClient_rss_feed($lng =NULL, $origin =NULL, $project =NULL) {
  // Get the translation language.
  $lng = variable_get('btrClient_translation_lng', 'fr');

  // Get the domain of the feed in the form of 'origin/project'
  // (or just 'origin').
  $domain = '';
  if ( ! empty($origin) ) {
    $domain = $origin;
    if ( ! empty($project) ) {
      $domain .= '/' . $project;
    }
  }

  // Get the feed data from cache, if possible.
  $cid = "btrClient_rss_feed:$lng:$domain";
  $cache = cache_get($cid, 'cache_btrClient');
  if (!empty($cache) && isset($cache->data) && !empty($cache->data)) {
    $rss_feed = $cache->data;
    drupal_add_http_header('Content-Type', 'application/rss+xml; charset=utf-8');
    print $rss_feed;
    return;
  }

  // Get the latest translations from the B-Translator server.
  $btr_server = variable_get('btrClient_server', NULL);
  if (empty($btr_server)) {
    $translations = array();
  }
  else {
    $url = $btr_server . "/translations/latest/$lng/$origin/$project";
    $output = drupal_http_request($url);
    $translations = json_decode($output->data, TRUE);
  }

  // Start building the RSS feed output.
  $rss_feed = '<?xml version="1.0" encoding="utf-8"?>' . "\n";
  $rss_feed .= '<rss version="2.0">';

  $link = url("translations/rss-feed/$lng", array('absolute' => TRUE));
  if (!empty($domain)) {
    $link .= '/' . $domain;
  }
  $title = variable_get('site_name', 'B-Translator');
  if (empty($domain)) {
    $description = t('Latest translation suggestions on B-Translator');
  }
  else {
    $description = t('Latest translation suggestions on B-Translator, for @domain', array('@domain' => $domain));
  }
  $ttl = 180;  // 3 hours = 3*60 min
  $site_mail = variable_get('site_mail');
  $site_name = variable_get('site_name');
  $webMaster = "$site_mail ($site_name)";

  $lastBuildDate = date('r');
  $rss_feed .= "
    <channel>
      <title>$title</title>
      <link>$link</link>
      <description>$description</description>
      <category>B-Translator</category>
      <language>$lng</language>
      <ttl>$ttl</ttl>
      <webMaster>$webMaster</webMaster>
      <lastBuildDate>$lastBuildDate</lastBuildDate>
    ";

  // Append an item for each translation.
  $base_url = url('translations', array('absolute' => TRUE));
  foreach ($translations as $translation) {
    extract($translation);
    $title = btrClient_shorten($string, 60);
    $link = $base_url . "/$lng/$sguid";
    $description = "<xmp>$string</xmp> -- <xmp>$translation</xmp>";
    $pubDate = date('r', strtotime($time));
    $rss_feed .= "
      <item>
        <title>$title</title>
        <link>$link</link>
        <description><![CDATA[$description]]></description>
        <author>$umail ($name)</author>
        <category>$origin</category>
        <category>$project</category>
        <guid isPermaLink='false'>$tguid</guid>
        <pubDate>$pubDate</pubDate>
      </item>
    ";
  }
  // Close the XML elements.
  $rss_feed .= "</channel>\n";
  $rss_feed .= "</rss>\n";

  // Cache for 2 hours.
  cache_set($cid, $rss_feed, 'cache_btrClient', strtotime('+2 hours'));

  // Send the output.
  drupal_add_http_header('Content-Type', 'application/rss+xml; charset=utf-8');
  print $rss_feed;
}
