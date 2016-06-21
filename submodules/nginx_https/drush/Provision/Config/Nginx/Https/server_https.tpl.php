<?php include(provision_class_directory('Provision_Config_Nginx_Server') . '/server.tpl.php'); ?>

#######################################################
###  nginx default ssl server
#######################################################

<?php
// TODO: Check/document what "satellite mode" is.
$satellite_mode = drush_get_option('satellite_mode');
if (!$satellite_mode && $server->satellite_mode) {
  $satellite_mode = $server->satellite_mode;
}
?>

server {

<?php foreach ($server->ip_addresses as $ip) :  /* TODO: remove this*/?>
  listen       <?php print $ip . ':' . $http_ssl_port; ?>;
<?php endforeach; ?>
  server_name  _;
  location / {
<?php if ($satellite_mode == 'boa'): /* TODO: Remove BOA-specific settings. Find ways to re-implement them via hooks, etc.*/?>
    root   /var/www/nginx-default;
    index  index.html index.htm;
<?php else: ?>
    return 404;
<?php endif; ?>
  }
}
