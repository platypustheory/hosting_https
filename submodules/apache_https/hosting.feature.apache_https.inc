<?php
/**
 * @file Register the Apache HTTPS Hosting feature.
 */

/**
 * Implements hook_hosting_feature().
 */
function hosting_apache_https_hosting_feature() {
  $features['apache_https'] = array(
    'title' => t('Apache HTTPS support'),
    'description' => t('Provide support for the Apache web server with HTTPS functionality.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_apache_https',
    'group' => 'experimental',
  );
  return $features;
}
