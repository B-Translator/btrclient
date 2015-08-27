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
  $import = bcl::get_block('btrProject', 'import_translations');
  $import['btrProject_import_translations']['import_translations']['#collapsible'] = TRUE;
  $import['btrProject_import_translations']['import_translations']['#collapsed'] = TRUE;
  $output[] = $import;

  if (bcl::user_access('btranslator-import')) {
    $import = bcl::get_block('btrProject', 'import');
    $import['btrProject_import']['import']['#collapsible'] = TRUE;
    $import['btrProject_import']['import']['#collapsed'] = TRUE;
    $output[] = $import;
  }

  return $output;
}
