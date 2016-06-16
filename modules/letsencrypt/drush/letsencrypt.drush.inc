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

