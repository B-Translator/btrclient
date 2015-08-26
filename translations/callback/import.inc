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
  return bcl::get_block('btrProject', 'import_translations');
}

/**
 * Menu callback for 'translations/import_project'.
 * Import strings and translations from PO files.
 * It will create/update a project (both strings and translations).
 */
function btrTranslations_importproject() {
  return bcl::get_block('btrProject', 'import');
}
