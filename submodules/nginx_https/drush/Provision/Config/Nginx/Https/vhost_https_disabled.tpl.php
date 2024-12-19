
<?php if ($this->https_enabled && $this->https_key && $this->https_cert_ok) : ?>

<?php

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
 else {
      if ($nginx_has_http2) {
         $ssl_args = "ssl http2";
      }
      else {
           $ssl_args = "ssl";
           }
      $listen_on = "listen *:" . $https_port . " " . $ssl_args .";\n";
      $ssl_on = "ssl 2  on;\n";
      $http2_on = "";
 }
?>

server {
 <?php print $listen_on ?>
 <?php print $ssl_on ?>
 <?php print $http_on ?>

  server_name  <?php print $this->uri . ' ' . implode(' ', str_replace('/', '.', $this->aliases)); ?>;
<?php if ($satellite_mode == 'boa'): /* TODO: Remove BOA-specific config. */?>
  root         /var/www/nginx-default;
  index        index.html index.htm;
  ### Do not reveal Aegir front-end URL here.
<?php else: ?>
  return 302 <?php print $this->platform->server->web_disable_url . '/' . $this->uri ?>;
<?php endif; ?>
  ssl_certificate_key        <?php print $https_cert_key; ?>;
<?php if (!empty($https_chain_cert)) : ?>
  ssl_certificate            <?php print $https_chain_cert; ?>;
<?php else: ?>
  ssl_certificate            <?php print $https_cert; ?>;
<?php endif; ?>
}

<?php endif; ?>

<?php
  // Generate the standard virtual host too.
  include(provision_class_directory('Provision_Config_Nginx_Site') . '/vhost_disabled.tpl.php');
?>
