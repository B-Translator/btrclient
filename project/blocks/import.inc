<?php
/**
 * @file
 * Import translations.
 */

/**
 * Called from hook_block_view for import.
 */
function btrProject_import_view() {
  // Get the arguments from the path.
  list($origin, $project, $lng) = bcl::get_project_from_path();

  $block['subject'] = t('Import Project');
  $block['content'] = drupal_get_form('btrProject_import_form', $origin, $project, $lng);
  return $block;
}

/**
 * Form callback for: btrProject_import_form
 */
function btrProject_import_form($form, &$form_state, $origin, $project, $lng) {
  $description = t('
<p>This form is for creating/updating custom translation projects. The PO/POT files
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
strings to them) and certain assumptions are made about them.</p> ',
                 ['!lng' => bcl::get_translation_lng()]
  );

  $form = [
    'import' => [
      '#type' => 'fieldset',
      '#title' => t('Create a project or update an existing one by importing POT/PO files'),
      '#description' => $description,
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,

      'origin' => [
        '#type' => 'textfield',
        '#title' => t('Origin'),
        '#default_value' => 'custom',
        '#required' => TRUE,
        //'#attributes' => array('readonly' => 'readonly'),
      ],

      'project' => [
        '#type' => 'textfield',
        '#title' => t('Project'),
        '#required' => TRUE,
      ],

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
 * Implements hook form_validate() for btrProject_import_form.
 */
function btrProject_import_form_validate($form, &$form_state) {
  $extensions = 'pot po tar gz tgz bz2 xz 7z zip';
  bcl::uploadfile_validate($extensions);
}


/**
 * Implement hook form_submit() for the form btrProject_import_form
 */
function btrProject_import_form_submit($form, &$form_state) {
  if (!bcl::user_is_authenticated()) {
    bcl::user_authenticate($form_state, $redirection = ($form===NULL));
    return;
  }

  $messages = bcl::uploadfile_to_server('project/import', [
                'origin' => $form_state['values']['origin'],
                'project' => $form_state['values']['project'],
              ]);
  bcl::display_messages($messages);
}
