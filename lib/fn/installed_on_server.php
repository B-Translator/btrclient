<?php
/**
 * @file
 * Function: installed_on_server()
 */

namespace BTranslator\Client;

/**
 * Return true if btrClient is installed on B-Translator server.
 */
function installed_on_server() {
  return module_exists('btrCore');
}
