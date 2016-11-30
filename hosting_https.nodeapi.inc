<?php
/**
 * @file
 * NodeAPI functions for the Hosting HTTPS module.
 */

/**
 * Form API code to extend the site form with HTTPS fields.
 */
function hosting_https_site_form(&$form, &$form_state, $form_id) {
  $node = $form['#node'];

  $https_available = FALSE;
  // Only allow the user to modify these values when the platform is HTTPS enabled.
  if (isset($node->nid)) {
    $platform = node_load($node->platform);
    $server = node_load($platform->web_server);

    if ($server->services['http']->https_enabled) {
      $https_available = TRUE;
    }
  }
  elseif (count(hosting_https_get_servers('http')) > 0) {
    $https_available = TRUE;
  }

  if (!$https_available) {
    return;
  }

  _hosting_site_field($form, $node, 'hosting_https_wrapper', array(
    '#type' => 'fieldset',
    '#title' => t('HTTPS Settings'),
    '#default_value' => NULL,
  ), 'filter_xss', $https_available);

  _hosting_site_field($form['hosting_https_wrapper'], $node, 'https_enabled', array(
    '#type' => 'radios',
    '#title' => t('Encryption'),
    '#options' => hosting_https_status_options(),
    '#description' => t('Enabling encryption will publish your site on both HTTP and HTTPS ports, allowing you to redirect users to the more secure version for certain pages that require the additional security. Requiring encryption will automatically redirect all unencrypted traffic to your HTTPS site.') . '<br/><strong>' . t("If the server is setup to generate the HTTPS certificate using Let's Encrypt, ensure that there is an existing public DNS entry for the site, otherwise installation will fail.") . '</strong>',
    '#required' => TRUE,
    '#default_value' => (isset($node->https_enabled)) ? $node->https_enabled : HOSTING_HTTPS_DISABLED,
    '#access' => user_access('manage site HTTPS settings'),
  ), 'hosting_https_status_options', $https_available);

  // Add our custom validate callback.
  $form['#validate'][] = 'hosting_https_site_form_validate';
}

/**
 * Implements hook_hosting_site_options_alter().
 */
function hosting_https_hosting_site_options_alter(&$return, $node) {
  // Test if HTTPS has been enabled.
  if (isset($node->https_enabled) && $node->https_enabled) {
    // we need to ensure that the return value is properly indexed, otherwise it
    // gets interpreted as an object by jquery.
    $return['profile'] = array_values(array_intersect($return['profile'], hosting_https_get_profiles()));

    $return['platform'] = array_values(array_intersect($return['platform'], hosting_https_get_platforms()));
  }
}

/**
 * Implements hook_nodeapi_TYPE_OP().
 */
function hosting_https_nodeapi_site_view(&$node, $teaser = FALSE) {
  $node->content['info']['https_enabled'] = array(
    '#type' => 'item',
    '#title' => t('Encryption'),
    '#markup' => hosting_https_status_options((isset($node->https_enabled)) ? $node->https_enabled : HOSTING_HTTPS_DISABLED),
    '#weight' => 6,
  );

  if (isset($node->https_enabled) && $node->https_enabled == TRUE) {
    $node->content['info']['https_key'] = array(
      '#type' => 'item',
      '#title' => t('Encryption key'),
      '#markup' => hosting_https_output_key($node),
      '#weight' => 7,
    );
  }
}

/**
 * Return the URI of a site, for use in generating HTTPS certificates.
 */
function hosting_https_get_key($node) {
  return hosting_https_filter_key($node->title);
}

/**
 * Filter HTTPS key for display.
 */
function hosting_https_output_key($node) {
  return filter_xss(hosting_https_get_key($node));
}

/**
 * Output filter for the HTTPS enabled field.
 */
function hosting_https_status_options($status = NULL) {
  $options = array(
    HOSTING_HTTPS_DISABLED => t('Disabled'),
    HOSTING_HTTPS_ENABLED => t('Enabled'),
    HOSTING_HTTPS_REQUIRED => t('Required'),
  );

  // Return a single label matching the requested status.
  if (!is_null($status)) {
    return $options[$status];
  }
  // Return the full mapping, for use as radio options.
  return $options;

}

/**
 * Filter disallowed characters from a HTTPS certificate key.
 *
 * Only lowercase alphanumeric- and '.', '_' or '-' characters are allowed for
 * HTTPS keys.
 *
 * TODO: Validate that site URIs will work here. For example, do sub-directory
 * sites need to be munged? example.com/site1 -> example.com_site1 ?
 */
function hosting_https_filter_key($key) {
  return strtolower(preg_replace("/[^\w\.\-]/", "", $key));
}

/**
 * Site form validation callback.
 *
 * @see hosting_https_site_form().
 */
function hosting_https_site_form_validate($form, $form_state) {
  if ($form_state['values']['https_enabled']) {
    $node = $form['#node'];
    // Check that the server to host this site has HTTPS enabled.
    if (!empty($node->platform)) {
      $platform = node_load($node->platform);
      $server = node_load($platform->web_server);
      if (!$server->services['http']->https_enabled) {
        $server_name = strlen($server->human_name) ? $server->human_name : $server->title;
        $server_edit_path = 'node/' . $server->nid . '/edit';
        if (drupal_valid_path($server_edit_path, TRUE)) {
          $enable = t('Enable HTTPS on !server.', array('!server' => l($server_name, $server_edit_path)));
        }
        else {
          $enable = t('Ask your site administrator to enable HTTPS.');
        }
        return form_set_error('hosting_https_wrapper',
          t("This site's platform is installed on a server that does not have HTTPS enabled. !enable", array(
            '!enable' => $enable,
        )));
      }
    }
  }
}

/**
 * Implements hook_nodeapi_TYPE_OP().
 */
function hosting_https_nodeapi_site_presave(&$node) {
  if (!isset($node->https_enabled)) {
    $node->https_enabled = HOSTING_HTTPS_DISABLED;
  }
}

/**
 * Implements hook_nodeapi_TYPE_OP().
 */
function hosting_https_nodeapi_site_insert($node) {
  $id = db_insert('hosting_https_site')
    ->fields(array(
      'vid' => $node->vid,
      'nid' => $node->nid,
      'https_enabled' => $node->https_enabled,
    ))
    ->execute();
}

/**
 * Implements hook_nodeapi_TYPE_OP().
 */
function hosting_https_nodeapi_site_update($node) {
  // Check if there's existing record.
  $result = db_query("SELECT https_enabled FROM {hosting_https_site} WHERE vid = :vid", array(':vid' => $node->vid));
  if (!($obj = $result->fetch())) {
    hosting_https_nodeapi_site_insert($node);
  }
  else {
    db_update('hosting_https_site')
      ->fields(array(
        'https_enabled' => $node->https_enabled,
      ))
      ->condition('vid', $node->vid)
      ->execute();
  }
}

/**
 * Implements hook_node_load().
 */
function hosting_https_node_load($nodes, $types) {
  foreach ($nodes as $nid => $node) {
    $result = db_query("SELECT https_enabled FROM {hosting_https_site} WHERE vid = :vid",
      array(':vid' => $node->vid)
      )->fetchObject();
    if ($result) {
      $nodes[$nid]->https_enabled = $result->https_enabled;
      $nodes[$nid]->https_key = hosting_https_get_key($node);
    }
  }
}

/**
 * Implements hook_nodeapi_TYPE_OP().
 */
function hosting_https_nodeapi_site_delete($node) {
  db_delete('hosting_https_site')
    ->condition('nid', $node->nid)
    ->execute();
}

/**
 * Implements hook_nodeapi_TYPE_OP().
 */
function hosting_https_nodeapi_site_delete_revision($node) {
  db_delete('hosting_https_site')
    ->condition('vid', $node->vid)
    ->execute();
}

