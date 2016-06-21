<?php
/**
 * @file Register the NGINX HTTPS Hosting feature.
 */

/**
 * Implements hook_hosting_feature().
 */
function hosting_nginx_https_hosting_feature() {
  $features['nginx_https'] = array(
    'title' => t('Nginx HTTPS support'),
    'description' => t('Provide support for the NGINX web server with HTTPS functionality.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_nginx_https',
    'group' => 'experimental',
  );
  return $features;
}
