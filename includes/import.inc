<?php
/**
 * @file
 * Import translations from PO files.
 * It is like bulk translation and voting.
 */

/**
 * Menu callback for 'translations/import'.
 */
function btrClient_import() {
  return drupal_get_form('btrClient_import_form');
}

/**
 * Import form.
 */
function btrClient_import_form($form, &$form_state) {
  $form = array(
    '#prefix' => '<div class="bcl-ui-filter clear-block">',
    '#suffix' => '</div>',
  );

  $form['import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Import PO files'),
    '#description' => t('
<p>Importing translations from PO files is like a bulk translation and
voting service. For any translation in the PO files, it will be added
as a suggestion if such a translation does not exist, or it will just
be voted if such a translation already exists. In case that the
translation already exists but its author is not known, then you (the
user who makes the import) will be recorded as the author of the
translation.</p>

<p>This functionality can be useful if you prefer to work off-line
with PO files. You can export the PO files of a project, work on them
with desktop tools (like Lokalize) to translate or correct exported
translations, and then import back to B-Translator the
translated/corrected PO files.</p>
'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  $form['import']['file'] = array(
    '#type' => 'file',
    '#description' => t('Upload a PO file. If you have many files, upload them as an archive (tar, tgz, bz2, 7z, zip).'),
  );

  $form['buttons'] = array(
    // '#prefix' => '<div class="import-submit">',
    // '#suffix' => '</div>',
  );
  $form['buttons']['submit'] = array(
    '#value' => t('Upload'),
    '#type' => 'submit',
  );

  return $form;
}

/**
 * Implements hook form_validate() for btrClient_import_form.
 */
function btrClient_import_form_validate($form, &$form_state, $extensions =NULL) {
  // Check that a file is uploaded.
  $file_tmp_name = $_FILES['files']['tmp_name']['file'];
  if ($file_tmp_name == '') {
    form_set_error('file', t("Please select a file."));
    return;
  }

  // Check for any other upload errors.
  $file_error = $_FILES['files']['error']['file'];
  if ($file_error != 0) {
    form_set_error('file',
      t("Error %error happened during file upload.",
        array('%error' => $file_error))
    );
    return;
  }

  // Check the extension of the uploaded file.
  if ($extensions === NULL)   $extensions = 'po tar gz tgz bz2 xz 7z zip';
  $regex = '/\.(' . preg_replace('/ +/', '|', preg_quote($extensions)) . ')$/i';
  $file_name = $_FILES['files']['name']['file'];
  if (!preg_match($regex, $file_name)) {
    form_set_error('file',
      t('Only files with the following extensions are allowed: %files-allowed.',
        array('%files-allowed' => $extensions))
    );
    return;
  }

  // Check the type of the uploaded file.
  $file_type = $_FILES['files']['type']['file'];
  $known_file_types = array(
    'text/x-gettext-translation',
    'application/x-tar',
    'application/x-compressed-tar',
    'application/x-bzip',
    'application/x-gzip',
    'application/x-7z-compressed',
    'application/x-xz',
    'application/zip',
  );
  if (!in_array($file_type, $known_file_types)) {
    form_set_error('file',
      t('File has unknown type: %file_type.',
        array('%file_type' => $file_type))
    );
    return;
  }
}

/**
 * Implements hook form_submit() for btrClient_import_form.
 */
function btrClient_import_form_submit($form, &$form_state) {
  $messages = _btrClient_upload_file(
    'translations/import',
    array(
      'lng' => variable_get('btrClient_translation_lng', 'fr'),
    )
  );
  btrClient_display_messages($messages);
}

/**
 * Make an http request for uploading a file.
 *
 * This is done manually with curl because wsclient cannot handle it.
 *
 * TODO: Replace this function and wsclient by Guzzle
 *       (requires PHP 5.4).
 */
function _btrClient_upload_file($endpoint, $params = array()) {
  $btr = wsclient_service_load('btr');

  // Get an access_token.
  module_load_include('inc', 'oauth2_client', 'oauth2_client');
  $oauth2_settings = $btr->settings['authentication']['oauth2'];
  $oauth2_client = new OAuth2\Client($oauth2_settings);
  try {
    $access_token = $oauth2_client->getAccessToken();
  }
  catch (Exception $e) {
    drupal_set_message($e->getMessage(), 'error');
    return;
  }

  // Get the details of the uploaded file.
  $file_name = $_FILES['files']['name']['file'];
  $file_tmp_name = $_FILES['files']['tmp_name']['file'];
  $file_size = $_FILES['files']['size']['file'];

  // Make an http request with curl.
  $ch = curl_init($btr->url . $endpoint);
  curl_setopt_array($ch, array(
      CURLOPT_POST => TRUE,
      CURLOPT_POSTFIELDS => array(
        'file' => "@$file_tmp_name;filename=$file_name",
      ) + $params,
      CURLOPT_HTTPHEADER => array(
        'Content-Type: multipart/form-data',
        'Authorization: Bearer ' . $access_token,
        'Accept: application/json',
      ),
      CURLOPT_RETURNTRANSFER => TRUE,
    ) +
    $btr->settings['curl options']
  );
  $result = curl_exec($ch);

  // Check for any errors and get the result.
  if (curl_errno($ch)) {
    $messages = array(
      array(curl_error($ch), 'error'),
    );
  }
  else {
    $result = json_decode($result, TRUE);
    $messages = $result['messages'];
  }
  curl_close($ch);

  return $messages;
}


/*============== Import Projet ===============*/

/**
 * Menu callback for 'translations/import_project'.
 * Import strings and translations from a PO file.
 * It will create/update a project (both strings and translations).
 */
function btrClient_importproject() {
  return drupal_get_form('btrClient_importproject_form');
}

/**
 * ImportProject form.
 */
function btrClient_importproject_form($form, &$form_state) {
  $form = array(
    '#prefix' => '<div class="btr-ui-filter clear-block">',
    '#suffix' => '</div>',
  );

  $form['import'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create/Update a Project'),
    '#description' => t('
<p>Create a custom project or update an existing one. The POT files
that are uploaded will be used for importing strings. PO files can be
uploaded instead of POT files, however only their strings will be used,
any translations will be ignored.</p>
'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );

  $form['import']['origin'] = array(
    '#type' => 'textfield',
    '#title' => t('Origin'),
    '#default_value' => 'custom',
    '#required' => TRUE,
    '#attributes' => array('readonly' => 'readonly'),
  );

  $form['import']['project'] = array(
    '#type' => 'textfield',
    '#title' => t('Project'),
    '#required' => TRUE,
  );

  $form['import']['file'] = array(
    '#type' => 'file',
    '#title' => t('POT File(s)'),
    '#description' => t('Upload a POT/PO file. If the project has several template files, upload them as an archive (tar, tgz, bz2, 7z, zip).'),
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
 * Implements hook form_validate() for btrClient_importproject_form.
 */
function btrClient_importproject_form_validate($form, &$form_state) {
  $extensions = 'pot po tar gz tgz bz2 xz 7z zip';
  btrClient_import_form_validate($form, $form_state, $extensions);
}

/**
 * Implements hook form_submit() for btrClient_importproject_form.
 */
function btrClient_importproject_form_submit($form, &$form_state) {
  $messages = _btrClient_upload_file(
    'project/import',
    array(
      'project' => $form_state['values']['project'],
    )
  );
  btrClient_display_messages($messages);
}
