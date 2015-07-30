<?php

/**
 * Append the social share buttons and comments to the form.
 */
function _share_and_comment(&$form, $path, $title) {
  // Add RRSSB share buttons.
  $form['rrssb'] = array(
    '#markup' => bcl::rrssb_get_buttons(),
  );

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
