<?php

/**
 * @file
 *   A Letsencrypt implementation of the Certificate service for the Provision API.
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
 * Implements hook_provision_apache_server_config().
 *
 * @see https://github.com/lukas2511/letsencrypt.sh/blob/master/docs/wellknown.md
 */
function letsencrypt_provision_apache_server_config($data) {
  if (d()->Certificate_service_type == 'Letsencrypt' && $challenge_path = d()->letsencrypt_challenge_path) {
    drush_log(dt("Injecting Let's Encrypt 'well-known' ACME challenge directory ':path' vhost entry.", array(':path' => $challenge_path)));
    return "
Alias /.well-known/acme-challenge {$challenge_path}

<Directory {$challenge_path}>
        Options None
        AllowOverride None

        # Apache 2.x
        <IfModule !mod_authz_core.c>
                Order allow,deny
                Allow from all
        </IfModule>

        # Apache 2.4
        <IfModule mod_authz_core.c>
                Require all granted
        </IfModule>
</Directory>
";
  }
}

/**
 * Implements hook_provision_nginx_server_config().
 *
 * @see https://github.com/lukas2511/letsencrypt.sh/blob/master/docs/wellknown.md
 * @TODO: Test this.
 */
function letsencrypt_provision_nginx_server_config($data) {
  if (d()->Certificate_service_type == 'Letsencrypt' && $challenge_path = d()->letsencrypt_challenge_path) {
    $challenge_path = d()->letsencrypt_challenge_path;
    drush_log(dt("Injecting Let's Encrypt 'well-known' ACME challenge directory ':path' vhost entry.", array(':path' => $challenge_path)));
    return "
  location /.well-known/acme-challenge {
    alias {$challenge_path}
  }
";
  }
}

