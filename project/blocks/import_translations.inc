<?php
/**
 * @file
 * Import translations.
 */

/**
 * Called from hook_block_view for import_translations.
 */
function btrProject_import_translations_view() {
  // Get the arguments from the path.
  list($origin, $project, $lng) = bcl::get_project_from_path();

  $block['subject'] = t('Import Translations');
  $block['content'] = drupal_get_form('btrProject_import_translations_form', $origin, $project, $lng);
  return $block;
}

/**
 * Form callback for: btrProject_import_translations_form
 */
function btrProject_import_translations_form($form, &$form_state, $origin, $project, $lng) {
  $description = t('
<p>Importing PO files is for bulk translation and voting. For any translation in
the PO files, it will be added as a suggestion if such a translation does not
exist, or it will just be voted if such a translation already exists. In case
that the translation already exists but its author is not known, then you (the
user who makes the import) will be recorded as the author of the
translation.</p>

<p>This functionality can be useful if you prefer to work off-line with PO
files. You can export the PO files of a project, work on them with desktop
tools (like Lokalize) to translate or correct exported translations, and then
import back to B-Translator the translated/corrected PO files.</p>
');

  $form = [
    'import_translations' => [
      '#type' => 'fieldset',
      '#title' => t('Import translations from PO files'),
      '#description' => $description,
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,

      'file' => [
        '#type' => 'file',
        '#title' => t('PO File(s)'),
        '#description' => t('Upload a PO file. If you have many files, upload them as an archive (tar, tgz, bz2, 7z, zip).'),
      ],

      'submit' => [
        '#value' => t('Import'),
        '#type' => 'submit',
      ],
    ],
  ];
  return $form;
}

/**
 * Implements hook form_validate() for btrProject_import_translations_form.
 */
function btrProject_import_translations_form_validate($form, &$form_state) {
  bcl::uploadfile_validate();
}


/**
 * Implement hook form_submit() for the form btrProject_import_translations_form
 */
function btrProject_import_translations_form_submit($form, &$form_state) {
  if (!bcl::user_is_authenticated()) {
    bcl::user_authenticate($form_state, $redirection = ($form===NULL));
    return;
  }

  $messages = bcl::uploadfile_to_server('translations/import', [
                'lng' => bcl::get_translation_lng(),
              ]);
  bcl::display_messages($messages);
}
