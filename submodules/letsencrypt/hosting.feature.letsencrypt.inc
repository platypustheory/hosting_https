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
  $modules = system_rebuild_module_data();
  $features['letsencrypt'] = array(
    'title' => t("Let's Encrypt service"),
    'description' => $modules['hosting_letsencrypt']->info['description'],
    'status' => HOSTING_FEATURE_DISABLED,
    'module' => 'hosting_letsencrypt',
    // Callback functions to execute on enabling or disabling this feature
    'enable' => 'hosting_letsencrypt_feature_enable_callback',
    'disable' => 'hosting_letsencrypt_feature_disable_callback',
    'group' => 'experimental'
    );
  return $features;
}
