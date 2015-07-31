<?php
/**
 * @file
 * Function translateform_meta()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * When there is only a single string displayed, we can add metatags
 * about the string, social share buttons, discussions/comments, etc.
 */
function translateform_meta($lng, $sguid, $string) {
  $form = array();

  // Get string properties: title, url, description, hashtags
  $properties = _get_string_properties($string);
  $properties['lng'] = $lng;

  // Set the page title.
  drupal_set_title($properties['title']);

  // Add metatags: og:title, og:description, og:image, etc.
  _add_metatags($properties);

  // Add RRSSB share buttons.
  $form['rrssb'] = array(
    '#markup' => bcl::rrssb_get_buttons(
      array(
        'buttons' => ['googleplus', 'linkedin', 'facebook', 'twitter', 'email'],
        'url' => $properties['url'],
        'title' => $properties['title'],
        'summary' => $properties['description'],
        'hashtags' => $properties['hashtags'],
        'lng' => $properties['lng'],
      )),
  );

  if (module_exists('disqus')) {
    // Define the disqus form element.
    $form['disqus'] = array(
      '#type' => 'disqus',
      '#disqus' => array(
        'domain' => variable_get('disqus_domain', 'dev-btranslator'),
        'status' => TRUE,
        'url' => $properties['url'],
        'title' => $properties['title'],
        'identifier' => "translations/$lng/$sguid",
        'developer' => variable_get('disqus_developer', '1'),
      ),
      '#weight' => 101,
    );
  }

  return $form;
}

/**
 * Return string properties as an associative array of:
 * title, url, description, hashtags, etc.
 * These will be used for metatags and for the social share buttons.
 */
function _get_string_properties($string) {
  // Get some context info from the path.
  $context = _get_context_info();

  // Get the page title.
  $str = strip_tags(check_plain($string['string']));
  if ($context['vocabulary']) {
    $title = $context['vocabulary'] . ': ' . $str;
  }
  elseif ($context['origin'] and $context['project']) {
    $title = $context['origin'] . '/' . $context['project'] . ': ' . $str;
  }
  else {
    $title = t('String') . ': ' . $str;
  }
  $title = bcl::shorten($title, 50);

  // Get the description.
  $description = $str;
  $arr_translations = array();
  foreach ($string['translations'] as $trans) {
    $arr_translations[] = strip_tags(check_plain($trans['translation']));
  }
  if (!empty($arr_translations)) {
    $description .= ' <==> ' . implode(' / ', $arr_translations);
  }
  if (isset($_GET['url'])) {
    $description .= ' (see: ' . check_url($_GET['url']) . ')';
  }

  // Get the url.
  $uri = substr(request_uri(), 1);
  if ($context['vocabulary']) {
    $arr_parts = explode('/', $uri);
    if (!$arr_parts[2]) {
      $arr_parts[2] = $str;
      $uri = implode('/', $arr_parts);
    }
  }
  $url = url($uri, ['absolute' => TRUE]);

  // Get hashtags.
  $arr_tags = array();
  if ($context['origin'] and $context['origin'] != 'vocabulary') {
    $arr_tags[] = '#' . $context['origin'];
  }
  if ($context['project']) {
    $arr_tags[] = '#' . $context['project'];
  }
  if ($context['vocabulary']) {
    $arr_tags[] = '#' . $context['vocabulary'];
  }
  $hashtags = implode(' ', $arr_tags);

  // Return properties.
  $properties = array(
    'title' => $title,
    'url' => $url,
    'description' => $description,
    'hashtags' => $hashtags,
  );
  return $properties;
}

/**
 * Return some context info from the path, as an associative array of origin,
 * project, and vocabulary.
 */
function _get_context_info() {
  $args = explode('/', current_path());
  if ($args[0] == 'btr' and $args[1] == 'project') {
    $origin = $args[2];
    $project = $args[3];
    $vocabulary = NULL;
  }
  elseif ($args[0] == 'vocabulary') {
    $origin = NULL;
    $project = NULL;
    $vocabulary = $args[1];
  }
  else {
    $origin = NULL;
    $project = NULL;
    $vocabulary = NULL;
  }

  return array(
    'origin' => $origin,
    'project' => $project,
    'vocabulary' => $vocabulary,
  );
}

/**
 * Add metatags: og:title, og:description, og:image, og:image:size, etc.
 */
function _add_metatags($properties) {
  // Add og:type
  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      "property" => "og:type",
      "content" => "website",
    ),
  );
  drupal_add_html_head($element, 'og_type');

  // Add og:title
  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      "property" => "og:title",
      "content" => $properties['title'],
    ),
  );
  drupal_add_html_head($element, 'og_title');

  // Add og:description
  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      "property" => "og:description",
      "content" => $properties['description'],
    ),
  );
  drupal_add_html_head($element, 'og_description');

  // Add og:url
  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      "property" => "og:url",
      "content" => $properties['url'],
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
      "content" => $properties['title'],
    ),
  );
  drupal_add_html_head($element, 'twitter_title');

  // twitter:description
  $element = array(
    '#tag' => 'meta',
    '#attributes' => array(
      "property" => "twitter:description",
      "content" => $properties['description'],
    ),
  );
  drupal_add_html_head($element, 'twitter_description');
}
