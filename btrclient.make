api = 2
core = 7.x

defaults[projects][subdir] = contrib

projects[ctools] = 1.4
projects[libraries] = 2.2
projects[entity] = 1.5

projects[http_client] = 2.x-dev

projects[wsclient][version] = 1.0
projects[wsclient][patch][] = https://drupal.org/files/wsclient-1285310-http_basic_authentication-14.patch
projects[wsclient][patch][] = https://drupal.org/files/issues/wsclient-2138617-oauth2_support.patch

projects[oauth2_login] = 1.x-dev

projects[features] = 2.2
projects[strongarm] = 2.0

projects[disqus] = 1.10
projects[srrssb] = 1.0
projects[inside_iframe] = 1.0

projects[ajax_chain_select][download][type] = git
projects[ajax_chain_select][download][url] = http://git.drupal.org/sandbox/SajjadZaheer/2407675.git
projects[ajax_chain_select][download][branch] = 7.x-1.x
projects[ajax_chain_select][patch][] = https://www.drupal.org/files/issues/string-id-2555501-1.patch
projects[ajax_chain_select][patch][] = https://www.drupal.org/files/issues/variable-rename-2555521-1.patch
projects[ajax_chain_select][patch][] = https://www.drupal.org/files/issues/fix-hidden-input-fields-2555595-1.patch
projects[ajax_chain_select][patch][] = https://www.drupal.org/files/issues/unset-hidden-values-2555633-1.patch
