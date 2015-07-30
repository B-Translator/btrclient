<?php

/**
 * Add metatags (og:title, og:description, og:image, og:image:size).
 */
function _metatags($string, $vocabulary = NULL) {
  // Get the title.
  $str = $string['string'];
  if ($vocabulary) {
    $str = t('Vocabulary') . " $vocabulary: " . $str;
  }
  else {
    $str = t('String') . ': ' . $str;
  }
  $title = bcl::shorten($str, 100);

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
