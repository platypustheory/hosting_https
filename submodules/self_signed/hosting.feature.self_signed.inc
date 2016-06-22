<?php

/**
 * @file
 * Expose the self_signed feature to hostmaster.
 */

/**
 * Implements hook_hosting_feature().
 *
 * Register the self_signed hosting feature with Aegir, initially this feature
 * will be disabled.
 */
function hosting_self_signed_hosting_feature() {
  $features['self_signed'] = array(
    'title' => t('Self-signed service'),
    'description' => t('Self-signed service to generate HTTPS self_signed certificates for Aegir-hosted sites.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_self_signed',
    // Callback functions to execute on enabling or disabling this feature
    'enable' => 'hosting_self_signed_feature_enable_callback',
    'disable' => 'hosting_self_signed_feature_disable_callback',
    'group' => 'experimental'
    );
  return $features;
}
