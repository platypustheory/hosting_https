<?php

/**
 * Base class for SSL enabled virtual hosts.
 *
 * This class primarily abstracts the process of making sure the relevant keys
 * are synched to the server when the config files that use them get created.
 */
class Provision_Config_Http_Https_Site extends Provision_Config_Http_Site {
  public $template = 'vhost_https.tpl.php';
  public $disabled_template = 'vhost_https_disabled.tpl.php';

  public $description = 'encrypted virtual host configuration';

  function write() {
    parent::write();

    if ($this->ssl_enabled && $this->ssl_key) {
      $path = dirname($this->data['ssl_cert']);
      // Make sure the ssl.d directory in the server ssl.d exists. 
      provision_file()->create_dir($path, 
      dt("SSL Certificate directory for %key on %server", array(
        '%key' => $this->ssl_key,
        '%server' => $this->data['server']->remote_host,
      )), 0700);

      // Copy the certificates to the server's ssl.d directory.
      provision_file()->copy(
        $this->data['ssl_cert_source'],
        $this->data['ssl_cert'])
        || drush_set_error('SSL_CERT_COPY_FAIL', dt('failed to copy SSL certificate in place'));
      provision_file()->copy(
        $this->data['ssl_cert_key_source'],
        $this->data['ssl_cert_key'])
        || drush_set_error('SSL_KEY_COPY_FAIL', dt('failed to copy SSL key in place'));
      // Copy the chain certificate, if it is set.
      if (!empty($this->data['ssl_chain_cert_source'])) {
	      provision_file()->copy(
          $this->data['ssl_chain_cert_source'],
          $this->data['ssl_chain_cert'])
        || drush_set_error('SSL_CHAIN_COPY_FAIL', dt('failed to copy SSL certficate chain in place'));
      }
      // Sync the key directory to the remote server.
      $this->data['server']->sync($path);
    }
  }

  /**
   * Remove a stale certificate file from the server.
   */
  function unlink() {
    parent::unlink();

    if ($this->ssl_enabled) {
      // TODO: Delete the certificate. Presumably this should look something like:
      // $this->server->service('Certificate')->delete_certificates($this->ssl_key);
    }
  }
  
}

