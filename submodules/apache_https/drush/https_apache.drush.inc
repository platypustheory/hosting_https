<?php

/**
 * @file
 *   Register with Provision autoloader.
 */

/**
 * Implements hook_drush_init().
 */
function https_apache_drush_init() {
  https_apache_provision_register_autoload();
}

/**
 * Register our directory as a place to find provision classes.
 */
function https_apache_provision_register_autoload() {
  static $loaded = FALSE;
  if (!$loaded) {
    $loaded = TRUE;
    provision_autoload_register_prefix('Provision_', dirname(__FILE__));
  }
}

/**
 * Implements hook_provision_apache_vhost_config().
 *
 * Adds HSTS headers and HTTPS client authentication to sites that require it.
 */
function https_apache_provision_apache_vhost_config($uri, $data) {
  $http_service = $data['server']->service('http');

  if ((d()->type != 'site') ||
      !isset($http_service->https_enabled) || !$http_service->https_enabled ||
      !isset(d()->https_enabled) || !d()->https_enabled) {
    return '';
  }

  $lines = array();
  // The 2 is a bit of a magic number, because we cant rely on the constants being available yet.
  // 2 == Only SSL is required, corresponding to HOSTING_SSL_REQUIRED and HOSTING_HTTPS_REQUIRED.
  if (d()->https_enabled == 2) {
    $lines[] = 'Header add Strict-Transport-Security "max-age=15768001"';
  }

  if (isset(d()->https_client_authentication_enabled) && d()->https_client_authentication_enabled) {
    $lines[] = "  ###################################";
    $lines[] = "  ### Aegir HTTPS (hosting_https) ###";
    $lines[] = "  ###################################";
    $lines[] = "  # Enable HTTPS client authentication as per site settings.";
    $lines[] = "  SSLVerifyCLient optional_no_ca";
    $lines[] = "  SSLVerifyDepth 1";
    $lines[] = "  SSLOptions +StdEnvVars +ExportCertData";
    $lines[] = "\n";
  }

  return implode("\n", $lines);
}
