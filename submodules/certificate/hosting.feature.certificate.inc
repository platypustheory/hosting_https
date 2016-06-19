<?php

/**
 * @file
 * Expose the certificate feature to hostmaster.
 */

/**
 * Implements hook_hosting_feature().
 *
 * Register the certificate hosting feature with Aegir, initially this feature
 * will be disabled.
 */
function hosting_certificate_hosting_feature() {
  $features['certificate'] = array(
    'title' => t('Certificate service'),
    'description' => t('Certificate service to generate SSL certificates for Aegir-hosted sites.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_certificate',
    // Callback functions to execute on enabling or disabling this feature
    'enable' => 'hosting_certificate_feature_enable_callback',
    'disable' => 'hosting_certificate_feature_disable_callback',
    'group' => 'experimental'
    );
  return $features;
}
