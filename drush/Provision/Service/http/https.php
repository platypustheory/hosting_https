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

      if ($ssl_key = $this->context->ssl_key) {
        // Retrieve the paths to the cert and key files.
        // they are generated if not found.
        $certs = $this->server->service('Certificate')->get_certificates($ssl_key);
        $data = array_merge($data, $certs);
      }
    }

    return $data;
  }

  /**
   * Assign the given site to a certificate to mark its usage.
   *
   * This is necessary for the backend to figure out when it's okay to
   * remove certificates.
   *
   * Should never fail unless the receipt file cannot be created.
   *
   * @return the path to the receipt file if allocation succeeded
   */
  static function assign_certificate_site($ssl_key, $site) {
    $path = $site->data['server']->http_ssld_path . "/" . $ssl_key . "/" . $site->uri . ".receipt";
    drush_log(dt("registering site %site with SSL certificate %key with receipt file %path", array("%site" => $site->uri, "%key" => $ssl_key, "%path" => $path)));
    if (touch($path)) {
      return $path;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Unallocate this certificate from that site.
   *
   * @return the path to the receipt file if removal was successful
   */
  static function free_certificate_site($ssl_key, $site) {
    if (empty($ssl_key)) return FALSE;
    $ssl_dir = $site->platform->server->http_ssld_path . "/" . $ssl_key . "/";
    // Remove the file system reciept we left for this file
    if (provision_file()->unlink($ssl_dir . $site->uri . ".receipt")->
        succeed(dt("Deleted SSL Certificate association receipt for %site on %server", array(
          '%site' => $site->uri,
          '%server' => $site->server->remote_host)))->status()) {
      if (!Provision_Service_http_ssl::certificate_in_use($ssl_key, $site->server)) {
        drush_log(dt("Deleting unused SSL directory: %dir", array('%dir' => $ssl_dir)));
        _provision_recursive_delete($ssl_dir);
        $site->server->sync($path);
      }
      return $path;
    }
    else {
      return FALSE;
    }
  }
  
  /**
   * Retrieve the status of a certificate on this server.
   *
   * This is primarily used to know when it's ok to remove the file.
   * Each time a config file uses the key on the server, it touches
   * a 'receipt' file, and every time the site stops using it,
   * the receipt is removed.
   *
   * This function just checks if any of the files are still present.
   */
  static function certificate_in_use($ssl_key, $server) {
    $pattern = $server->http_ssld_path . "/$ssl_key/*.receipt";
    return sizeof(glob($pattern));
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
