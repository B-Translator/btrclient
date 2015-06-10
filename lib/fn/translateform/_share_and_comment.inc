<?php

/**
 * Append the social share buttons and comments to the form.
 */
function _share_and_comment(&$form, $path, $title) {
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
