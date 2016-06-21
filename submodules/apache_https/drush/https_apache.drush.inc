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

