<?php
/**
 * @file
 * Function: filter_submit()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Handle the submit of the filter form.
 */
function filter_submit($form, &$form_state) {
  /*
  if ($form_state['values']['op'] == t('Reset')) {
    // Just go with the redirection flow => removes URL params.
    return;
  }
  */

  if ($form_state['values']['op'] == '<span class="glyphicon glyphicon-repeat"></span>') {
    // Redirect (with a GET request) keeping the relevant filters intact
    // in the URL.
    $form_state['redirect'] = array(
      $_GET['q'],
      array('query' => bcl::filter_get_query($form_state['values'])),
    );
    return;
  }
}
