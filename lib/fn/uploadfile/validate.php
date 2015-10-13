<?php
/**
 * @file
 * Validate the file uploaded from the import form.
 */

namespace BTranslator\Client;
use \bcl;


/**
 * Validate the file uploaded from the import form.
 */
function uploadfile_validate($extensions = NULL, $file_types = NULL) {
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
        ['%error' => $file_error])
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
        ['%files-allowed' => $extensions])
    );
    return;
  }

  // Check the type of the uploaded file.
  $file_type = $_FILES['files']['type']['file'];
  if ($file_type === NULL) {
    $file_types = [
      'text/x-gettext-translation',
      'application/x-tar',
      'application/x-compressed-tar',
      'application/x-bzip',
      'application/x-gzip',
      'application/x-7z-compressed',
      'application/x-xz',
      'application/zip',
    ];
  }
  if (!in_array($file_type, $file_types)) {
    form_set_error('file',
      t('File has unknown type: %file_type.',
        ['%file_type' => $file_type])
    );
    return;
  }
}
