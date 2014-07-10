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

projects[features] = 2.0
projects[strongarm] = 2.0

projects[disqus] = 1.10
projects[sharethis] = 2.6
projects[service_links] = 2.2
