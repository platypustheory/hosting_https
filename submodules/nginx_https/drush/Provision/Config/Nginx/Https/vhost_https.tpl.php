
<?php if ($this->https_enabled && $this->https_key && $this->https_cert_ok) : ?>

<?php

// Check if the function is already defined
if (!function_exists('get_nginx_version')) {
   // Function to get Nginx version
   function get_nginx_version() {
     $output = shell_exec('nginx -v 2>&1');
     if (preg_match('/nginx\/([0-9]+\.[0-9]+\.[0-9]+)/', $output, $matches)) {
         return $matches[1];
     }
     return null;
   }
}

$nginx_version = get_nginx_version();

$satellite_mode = drush_get_option('satellite_mode');
if (!$satellite_mode && isset($server->satellite_mode)) {
  $satellite_mode = $server->satellite_mode;
}

$nginx_has_http2 = drush_get_option('nginx_has_http2');
if (!$nginx_has_http2 && isset($server->nginx_has_http2)) {
  $nginx_has_http2 = $server->nginx_has_http2;
}

if (version_compare($nginx_version, '1.25.0', '>=')) {
    $ssl_args = "ssl";
    $listen_on = "listen *:" . $https_port . " " . $ssl_args .";\n";
    $http2_on = "http2 on;\n";
    $ssl_on = "";
    }
    else{
        if ($nginx_has_http2) {
           $ssl_args = "ssl http2";
           } else {
             $ssl_args = "ssl";
           }
           $listen_on = "listen *:" . $https_port . " " . $ssl_args .";\n";
           $ssl_on = "ssl on;\n";
           $http2_on = "";
    }

?>

<?php if ($this->redirection): ?>
<?php foreach ($this->aliases as $alias_url): ?>
server {
    <?php print $listen_on ?>
    <?php print $http2_on ?>
    <?php print $ssl_on ?>
<?php
  // if we use redirections, we need to change the redirection
  // target to be the original site URL ($this->uri instead of
  // $alias_url)
  if ($this->redirection && $alias_url == $this->redirection) {
    $this->uri = str_replace('/', '.', $this->uri);
    print "  server_name  {$this->uri};\n";
  }
  else {
    $alias_url = str_replace('/', '.', $alias_url);
    print "  server_name  {$alias_url};\n";
  }
?>
  ssl_certificate_key        <?php print $https_cert_key; ?>;
<?php if (!empty($https_chain_cert)) : ?>
  ssl_certificate            <?php print $https_chain_cert; ?>;
<?php else: ?>
  ssl_certificate            <?php print $https_cert; ?>;
<?php endif; ?>
  return 301 $scheme://<?php print $this->redirection; ?>$request_uri;
}
<?php endforeach; ?>
<?php endif; ?>

server {
  include       fastcgi_params;
  # Block https://httpoxy.org/ attacks.
  fastcgi_param HTTP_PROXY "";
  fastcgi_param MAIN_SITE_NAME <?php print $this->uri; ?>;
  set $main_site_name "<?php print $this->uri; ?>";
  fastcgi_param SCRIPT_FILENAME $document_root$fastcgi_script_name;
  fastcgi_param HTTPS on;
<?php
  // If any of those parameters is empty for any reason, like after an attempt
  // to import complete platform with sites without importing their databases,
  // it will break Nginx reload and even shutdown all sites on the system on
  // Nginx restart, so we need to use dummy placeholders to avoid affecting
  // other sites on the system if this site is broken.
  if (!$db_type || !$db_name || !$db_user || !$db_passwd || !$db_host) {
    $db_type = 'mysqli';
    $db_name = 'none';
    $db_user = 'none';
    $db_passwd = 'none';
    $db_host = 'localhost';
  }
?>
  fastcgi_param db_type   <?php print urlencode($db_type); ?>;
  fastcgi_param db_name   <?php print urlencode($db_name); ?>;
  fastcgi_param db_user   <?php print urlencode($db_user); ?>;
  fastcgi_param db_passwd <?php print urlencode($db_passwd); ?>;
  fastcgi_param db_host   <?php print urlencode($db_host); ?>;
<?php
  // Until the real source of this problem is fixed elsewhere, we have to
  // use this simple fallback to guarantee that empty db_port does not
  // break Nginx reload which results with downtime for the affected vhosts.
  if (!$db_port) {
    $db_port = $this->server->db_port ? $this->server->db_port : '3306';
  }
?>
  fastcgi_param db_port   <?php print urlencode($db_port); ?>;
  <?php print $listen_on ?>
  server_name   <?php
    // this is the main vhost, so we need to put the redirection
    // target as the hostname (if it exists) and not the original URL
    // ($this->uri)
    if ($this->redirection) {
      print str_replace('/', '.', $this->redirection);
    } else {
      print $this->uri;
    }
    if (!$this->redirection && is_array($this->aliases)) {
      foreach ($this->aliases as $alias_url) {
        if (trim($alias_url)) {
          print " " . str_replace('/', '.', $alias_url);
        }
      }
    } ?>;
  root          <?php print "{$this->root}"; ?>;
  <?php print $ssl_on ?>
  <?php print $http2_on ?>
  ssl_certificate_key        <?php print $https_cert_key; ?>;
<?php if (!empty($https_chain_cert)) : ?>
  ssl_certificate            <?php print $https_chain_cert; ?>;
<?php else: ?>
  ssl_certificate            <?php print $https_cert; ?>;
<?php endif; ?>
<?php print $extra_config; ?>
  include                    <?php print $server->include_path; ?>/nginx_vhost_common.conf;
}

<?php endif; ?>

<?php
  // Generate the standard virtual host too.
  include(provision_class_directory('Provision_Config_Nginx_Site') . '/vhost.tpl.php');
?>
