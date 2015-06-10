<?php
/**
 * @file
 * Function: user_get_profile()
 */

namespace BTranslator\Client;
use \bcl;

/**
 * Return the profile of the user on B-Translator server.
 */
function user_get_profile() {
  // Return the cached one, if possible.
  if (isset($_SESSION['btrClient']['btr_profile'])) {
    return $_SESSION['btrClient']['btr_profile'];
  }

  // Get the profile from the B-Translator server.
  $btr_profile = bcl::user_get_remote_profile();

  // Save it in session.
  $_SESSION['btrClient']['btr_profile'] = $btr_profile;

  return $btr_profile;
}
