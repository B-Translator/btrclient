<?php
/**
 * @file
 * Import translations from PO files.
 * It is like bulk translation and voting.
 */

/**
 * Menu callback for 'translations/import'.
 */
function btrTranslations_import() {
  $output = bcl::get_block('btrProject', 'import_translations');
  return $output;
}
