<?php
/**
 * @file
 * Export the best translations of a project into PO format.
 */

/**
 * Callback for translations/export.
 *
 * Export the best translations of a project into PO format.
 */
function btrTranslations_export() {
  $output = bcl::get_block('btrProject', 'export');
  return $output;
}
