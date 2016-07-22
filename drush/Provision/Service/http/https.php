<?php
/**
 * @file
 * The base implementation of the SSL capabale web service.
 */

/**
 * The base class for SSL supporting servers.
 *
 * In general, these function the same as normal servers, but have an extra
 * port and some extra variables in their templates.
 */
class Provision_Service_http_https extends Provision_Service_http_public {
  protected $ssl_enabled = TRUE;

  function default_ssl_port() {
    return 443;
  }

  function init_server() {
    parent::init_server();

    // SSL Port.
    $this->server->setProperty('http_ssl_port', $this->default_ssl_port());

    // SSL certificate store.
    // The certificates are generated from here, and distributed to the servers,
    // as needed.
    $this->server->ssld_path = "{$this->server->aegir_root}/config/ssl.d";

    // SSL certificate store for this server.
    // This server's certificates will be stored here.
    $this->server->http_ssld_path = "{$this->server->config_path}/ssl.d";
    $this->server->ssl_enabled = 1;
    $this->server->ssl_key = 'default';
  }

  function init_site() {
    parent::init_site();

    $this->context->setProperty('ssl_enabled', 0);
    $this->context->setProperty('ssl_key', NULL);
  }


  function config_data($config = NULL, $class = NULL) {
    $data = parent::config_data($config, $class);
    $data['http_ssl_port'] = $this->server->http_ssl_port;

    if ($config == 'server') {
      // Generate a certificate for the default SSL vhost, and retrieve the
      // path to the cert and key files. It will be generated if not found.
      $certs = $this->server->service('Certificate')->get_certificates('default');
      $data = array_merge($data, $certs);
    }

    if ($config == 'site' && $this->context->ssl_enabled) {
      if ($this->context->ssl_enabled == 2) {
        $data['ssl_redirection'] = TRUE;
        $data['redirect_url'] = "https://{$this->context->uri}";
      }

      if ($this->context->ssl_key) {
        // Retrieve the paths to the cert and key files.
        // They are generated if not found.
        $certs = $this->server->service('Certificate')->get_certificates($this->context->ssl_key);
        $data = array_merge($data, $certs);
      }
    }

    return $data;
  }

  /**
   * Verify server.
   */
  function verify_server_cmd() {
    if ($this->context->type === 'server') {
      provision_file()->create_dir($this->server->ssld_path, dt("Central SSL certificate repository."), 0700);

      provision_file()->create_dir($this->server->http_ssld_path,
        dt("SSL certificate repository for %server",
        array('%server' => $this->server->remote_host)), 0700);

      $this->sync($this->server->http_ssld_path, array(
        'exclude' => $this->server->http_ssld_path . '/*',  // Make sure remote directory is created
      ));
      $this->sync($this->server->http_ssld_path . '/default');
    }

    // Call the parent at the end. it will restart the server when it finishes.
    parent::verify_server_cmd();
  }

}
