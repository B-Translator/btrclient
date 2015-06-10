<?php
/**
 * @file
 * Function: inside_iframe()
 */

namespace BTranslator\Client;

/**
 * Return true if the page is being displayed inside an iframe.
 */
function inside_iframe() {
  return (isset($_GET['display']) and $_GET['display'] == 'iframe');
}
