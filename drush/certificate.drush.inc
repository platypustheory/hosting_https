<?php

/**
 * @file
 *   A certificate service for the Provision API.
 *
 *  Declares a new service type and a 'selfsigned' implementation of it. It
 *  matches the same service definition in the hosting front end.
 */

/**
 * Implements hook_drush_init().
 */
function certificate_drush_init() {
  certificate_provision_register_autoload();
}

/**
 * Register our directory as a place to find provision classes.
 */
function certificate_provision_register_autoload() {
  static $loaded = FALSE;
  if (!$loaded) {
    $loaded = TRUE;
    provision_autoload_register_prefix('Provision_', dirname(__FILE__));
  }
}

/**
 * Expose the service type this extension defines to provision.
 *
 * Implementations are then conditionally loaded when the option 
 * "--$service_service_type=$type" is passed to the provision-save
 * command of a server.
 *
 * Implementations are automatically loaded from :
 * provision/$service/$type/$type_service.inc.
 *
 * @return
 *   An array with the service type as key, and the default implementation as value.
 */
function certificate_provision_services() {
  certificate_provision_register_autoload();
  return array('certificate' => NULL);
}
