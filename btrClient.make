api = 2
core = 7.x

defaults[projects][subdir] = "contrib"

projects[ctools][version] = "1.4"
projects[libraries][version] = "2.2"
projects[entity][version] = "1.5"

projects[http_client][version] = "2.x-dev"
projects[wsclient][version] = "1.0"
projects[wsclient][patch][] = "https://drupal.org/files/wsclient-1285310-http_basic_authentication-14.patch"
projects[wsclient][patch][] = "https://drupal.org/files/issues/wsclient-2138617-oauth2_support.patch"

;projects[oauth2_login][version] = "1.0"
projects[oauth2_login][type] = "module"
projects[oauth2_login][download][type] = "git"
projects[oauth2_login][download][url] = "https://github.com/dashohoxha/oauth2_login.git"
;projects[oauth2_login][download][branch] = "7.x-1.x"

projects[features][version] = "1.0"
projects[strongarm][version] = "2.0"

projects[disqus][version] = "1.10"
projects[sharethis][version] = "2.6"
projects[service_links][version] = "2.2"
