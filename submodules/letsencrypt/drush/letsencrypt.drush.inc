<?php

/**
 * @file
 *   A LetsEncrypt implementation of the Certificate service for the Provision API.
 */

/**
 * Implements hook_drush_init().
 */
function letsencrypt_drush_init() {
  letsencrypt_provision_register_autoload();
}

/**
 * Register our directory as a place to find provision classes.
 */
function letsencrypt_provision_register_autoload() {
  static $loaded = FALSE;
  if (!$loaded) {
    $loaded = TRUE;
    provision_autoload_register_prefix('Provision_', dirname(__FILE__));
  }
}

/**
 * Implements hook_provision_services().
 *
 * Ensure our classes are loaded early enough to instantiate the Provision
 * services.
 */
function letsencrypt_provision_services() {
  letsencrypt_provision_register_autoload();
}

/**
 * Implements hook_provision_apache_vhost_config().
 *
 * @see https://github.com/lukas2511/dehydrated/blob/master/docs/wellknown.md
 */
function letsencrypt_provision_apache_vhost_config($uri, $data) {
  $server = d('@server_master');

  if (($server->Certificate_service_type == 'LetsEncrypt') && ($challenge_path = $server->letsencrypt_challenge_path)) {
    drush_log(dt("Injecting Let's Encrypt 'well-known' ACME challenge directory ':path' into Apache vhost entry.", array(
      ':path' => $challenge_path,
    )));

    $lines = array();
    $lines[] = "  # Allow access to letsencrypt.org ACME challenges directory.";
    $lines[] = "  Alias /.well-known/acme-challenge {$challenge_path}";
    $lines[] = "";
    $lines[] = "  <Directory {$challenge_path}>";
    $lines[] = "    Options None";
    $lines[] = "    AllowOverride None";
    $lines[] = "";
    $lines[] = "    # Apache 2.x";
    $lines[] = "    <IfModule !mod_authz_core.c>";
    $lines[] = "      Order allow,deny";
    $lines[] = "      Allow from all";
    $lines[] = "    </IfModule>";
    $lines[] = "";
    $lines[] = "    # Apache 2.4";
    $lines[] = "    <IfModule mod_authz_core.c>";
    $lines[] = "      Require all granted";
    $lines[] = "    </IfModule>";
    $lines[] = "  </Directory>";
    $lines[] = "\n";
    return implode("\n", $lines);
  }
}

/**
 * Implements hook_provision_nginx_vhost_config().
 *
 * @see https://github.com/lukas2511/dehydrated/blob/master/docs/wellknown.md
 */
function letsencrypt_provision_nginx_vhost_config($uri, $data) {
  $server = d('@server_master');

  if (($server->Certificate_service_type == 'LetsEncrypt') && ($challenge_path = $server->letsencrypt_challenge_path)) {
    drush_log(dt("Injecting Let's Encrypt 'well-known' ACME challenge directory ':path' into Nginx vhost entry.", array(
      ':path' => $challenge_path,
    )));

    $lines = array();
    $lines[] = "  # Allow access to letsencrypt.org ACME challenges directory.";
    $lines[] = "  location ^~ /.well-known/acme-challenge {";
    $lines[] = "    alias {$challenge_path};";
    $lines[] = "    try_files \$uri 404;";
    $lines[] = "  }";
    $lines[] = "\n";
    return implode("\n", $lines);
  }
}

