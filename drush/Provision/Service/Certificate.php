<?php

/**
 * The service type base class.
 */
class Provision_Service_Certificate extends Provision_Service {
  public $service = 'Certificate';

  /**
   * Called on provision-verify.
   */
  function verify() {
    $this->create_config(d()->type);
    $this->parse_configs();
  }

  /**
   * PUBLIC API
   */

  /**
   * Commonly something like running the restart_cmd or sending SIGHUP to a process.
   */
  function parse_configs() {
    return TRUE;
  }

  /**
   * Generate a site specific configuration file.
   */
  function create_site_config() {
    return TRUE;
  }

  /**
   * Remove an existing site configuration file.
   */
  function delete_site_config() {
    return TRUE;
  }

  /**
   * Add a new platform specific configuration file.
   */
  function create_platform_config() {
    return TRUE;
  }

  /**
   * Remove an existing platform configuration file.
   */
  function delete_platform_config() {
    return TRUE;
  }

  /**
   * Create a new server specific configuration file.
   */
  function create_server_config() {
    return TRUE;
  }

  /**
   * Remove an existing server specific configuration file.
   */
  function delete_server_config() {
    return TRUE;
  }
}
