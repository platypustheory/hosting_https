NameVirtualHost <?php print "*:" . $https_port . "\n"; ?>

<IfModule !ssl_module>
  LoadModule ssl_module modules/mod_ssl.so
</IfModule>

<VirtualHost *:443>
  SSLEngine on
  SSLCertificateFile <?php print $https_cert . "\n"; ?>
  SSLCertificateKeyFile <?php print $https_cert_key . "\n"; ?>
<?php if (!empty($https_chain_cert)) : ?>
  SSLCertificateChainFile <?php print $https_chain_cert . "\n"; ?>
<?php endif; ?>
  ServerName default
  Redirect 404 /
</VirtualHost>

<?php include(provision_class_directory('Provision_Config_Apache_Server') . '/server.tpl.php'); ?>
