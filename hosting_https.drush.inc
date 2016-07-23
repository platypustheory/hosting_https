<?php
/**
 * @file
 * Drush hooks for the Hosting HTTPS module.
 */

/**
 * Implements hook_hosting_TASK_OBJECT_context_options().
 */
function hosting_https_hosting_site_context_options(&$task) {
  $node = $task->ref;

  if (isset($node->https_enabled)) {
    // we pass null to disable the option.
    $task->context_options['https_enabled'] = ($node->https_enabled) ? $node->https_enabled : 'null';

    if ($node->https_enabled) {
      $task->context_options['https_key'] = hosting_https_get_key($node);
    }
    else {
      $task->context_options['https_key'] = 'null';
    }
  }
}
