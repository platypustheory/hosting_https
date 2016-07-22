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
    #$this->create_config(d()->type);
    #$this->parse_configs();
  }

  /**
   * PUBLIC API
   */

  /**
   * Return the path where we'll generate our certificates.
   */
  function get_source_path($ssl_key) {
    // Default to the ~/config/ssl.d directory.
    return "{$this->server->ssld_path}/{$ssl_key}";
  }

  /**
   * Retrieve an array containing the actual files for this ssl_key.
   *
   * If the files could not be found, this function will proceed to generate
   * certificates for the current site, so that the operation can complete
   * succesfully.
   */
  function get_certificates($ssl_key) {
    $certs = $this->get_certificate_paths($ssl_key);

    foreach ($certs as $cert) {
      $exists = provision_file()->exists($cert)->status();
      if (!$exists) {
        // if any of the files don't exist, regenerate them.
        $this->generate_certificates($ssl_key);

        // break out of the loop.
        break;
      }
    }

    // If a certificate chain file exists, add it.
    #$chain_cert_source = "{$source_path}/openssl_chain.crt";
    #if (provision_file()->exists($chain_cert_source)->status()) {
    #  $certs['ssl_chain_cert_source'] = $chain_cert_source;
    #  $certs['ssl_chain_cert'] = "{$path}/openssl_chain.crt";
    #}
    return $certs;
  }

  /**
   * Retrieve an array containing source and target paths for this ssl_key.
   */
  function get_certificate_paths($ssl_key) {
    // This is a dummy implementation. We should probably move this into an
    // interface.
    return TRUE;
  }

  /**
   * Generate a self-signed certificate for the provided key.
   */
  function generate_certificates($ssl_key) {
    // This is a dummy implementation. We should probably move this into an
    // interface.
    return TRUE;
  }

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
