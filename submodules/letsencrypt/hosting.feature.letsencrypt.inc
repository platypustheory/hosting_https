<?php

/**
 * @file
 * Expose the letsencrypt feature to hostmaster.
 */

/**
 * Implements hook_hosting_feature().
 *
 * Register the letsencrypt hosting feature with Aegir, initially this feature
 * will be disabled.
 */
function hosting_letsencrypt_hosting_feature() {
  $features['letsencrypt'] = array(
    'title' => t('Letsencrypt service'),
    'description' => t('Letsencrypt service to generate HTTPS letsencrypts for Aegir-hosted sites.'),
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_letsencrypt',
    // Callback functions to execute on enabling or disabling this feature
    'enable' => 'hosting_letsencrypt_feature_enable_callback',
    'disable' => 'hosting_letsencrypt_feature_disable_callback',
    'group' => 'experimental'
    );
  return $features;
}
