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
  return drupal_get_form('btrTranslations_import_form');
}


/*============== Import Project ===============*/

/**
 * Menu callback for 'translations/import_project'.
 * Import strings and translations from PO files.
 * It will create/update a project (both strings and translations).
 */
function btrTranslations_importproject() {
  return drupal_get_form('btrTranslations_importproject_form');
}

/**
 * ImportProject form.
 */
function btrTranslations_importproject_form($form, &$form_state) {
  $form = array(
    '#prefix' => '<div class="clear-block">',
    '#suffix' => '</div>',
  );

  $form['import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create a project or update an existing one'),
    '#description' => t('
<p>This is useful for creating custom translation projects. The PO/POT files
that are uploaded will be used for importing strings and translations. If
there are no POT files, then the PO files will be used both for importing
strings and for importing translations. If there are POT files and PO files,
their names have to match (except for the extension).</p>

<p>If you want to create a vocabulary, use <strong>vocabulary</strong> as the
origin of the project, and add the suffix \'<strong>_!lng</strong>\' to the
project name. Use <strong>msgctxt "project_name"</strong> as the context of each
string in the PO/POT file. Also the uploaded PO file must have the same name
as the project, for example <strong>ICT_sq.po</strong>. These restrictions are
needed because vocabularies are pseudo-projects (for example you can add
strings to them) and certain assumptions are made about them.</p>
', array('!lng' => variable_get('btrClient_translation_lng', 'fr'))),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  $form['import']['origin'] = array(
    '#type' => 'textfield',
    '#title' => t('Origin'),
    '#default_value' => 'custom',
    '#required' => TRUE,
    //'#attributes' => array('readonly' => 'readonly'),
  );

  $form['import']['project'] = array(
    '#type' => 'textfield',
    '#title' => t('Project'),
    '#required' => TRUE,
  );

  $form['import']['file'] = array(
    '#type' => 'file',
    '#title' => t('PO File(s)'),
    '#description' => t('Upload a PO file. If there are several files, upload them as an archive (tar, tgz, bz2, 7z, zip).'),
  );

  $form['buttons'] = array(
    //'#prefix' => '<div class="import-submit">',
    //'#suffix' => '</div>',
  );
  $form['buttons']['submit'] = array(
    '#value' => t('Import'),
    '#type' => 'submit',
  );

  return $form;
}

/**
 * Implements hook form_validate() for btrTranslations_importproject_form.
 */
function btrTranslations_importproject_form_validate($form, &$form_state) {
  $extensions = 'pot po tar gz tgz bz2 xz 7z zip';
  btrTranslations_import_form_validate($form, $form_state, $extensions);
}

/**
 * Implements hook form_submit() for btrTranslations_importproject_form.
 */
function btrTranslations_importproject_form_submit($form, &$form_state) {
  $messages = bcl::upload_file('project/import', [
                'origin' => $form_state['values']['origin'],
                'project' => $form_state['values']['project'],
              ]);
  bcl::display_messages($messages);
}
