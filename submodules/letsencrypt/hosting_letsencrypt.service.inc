<?php

/**
 * @file
 * Letsencrypt service implementation of the Certificate service type for the hosting front end.
 */

/**
 * An implementation of the certificate service type, registered with hook_hosting_service.
 */
class hostingService_Certificate_Letsencrypt extends hostingService_Certificate {

  public $type = 'Letsencrypt';

  protected $letsencrypt_field = 'default';

  /**
   * node operations
   */

  /**
   * Load associated values for the service.
   *
   * In this certificate we will use the variable system to retrieve values.
   */
  function load() {
    parent::load();
    $this->letsencrypt_field = variable_get('hosting_letsencrypt_field_' . $this->server->nid, $this->letsencrypt_field);

    /**
     * Although this certificate does not have it's own tables, we provide some utitilty functions
     * for use in this method.
     *
     * If this certificate used it's own tables, we could use the mergeData method below to merge in the
     * results automatically, instead of iterating through the results ourself.
     */

    // $this->mergeData("SELECT letsencrypt_field FROM {hosting_certificate} WHERE vid = :vid", array(':vid' => $this->server->vid));
  }

  /**
   * Display settings on the server node page.
   *
   * @param
   *   A reference to the associative array of the subsection of the page
   *   reserved for this service implementation.
   */
  function view(&$render) {
    parent::view($render);
    $render['letsencrypt_field'] = array(
      '#type' => 'item',
      '#title' => t("Let's Encrypt field"),
      // Remember to pass the display through filter_xss!
      '#markup' => filter_xss($this->letsencrypt_field),
    );
  }

  /**
   * Extend the server node form.
   *
   * @param
   *   A reference to the associative array of the subsection of the form
   *   reserved for this service implementation.
   */
  function form(&$form) {
    parent::form($form);

    $form['letsencrypt_field'] = array(
      '#type' => 'textfield',
      '#title' => t('Letsencrypt field'),
      '#description' => t('An letsencrypt field for the user to fill in.'),
      '#size' => 40,
      '#default_value' => $this->letsencrypt_field,
      '#maxlength' => 64,
      '#weight' => 5,
    );
  }

  /**
   * Validate a form submission.
   */
  function validate(&$node, &$form, &$form_state) {
    parent::validate($node, $form, $form_state);

    if (sizeof($this->letsencrypt_field) > 30) {
      form_set_error('letsencrypt_field', t("The certificate string must not be longer than 30 characters"));
    }
  }

  /**
   * Insert a record into the database.
   */
  function insert() {
    parent::insert();

    variable_set('hosting_letsencrypt_field_' . $this->server->nid, $this->letsencrypt_field);
  }

  /**
   * Update a record in the database.
   */
  function update() {
    parent::update();

    variable_set('hosting_letsencrypt_field_' . $this->server->nid, $this->letsencrypt_field);
  }

  /**
   * Delete a record from the database, based on server node.
   */
  function delete() {
    parent::delete();

    variable_del('hosting_letsencrypt_field_' . $this->server->nid);
  }

  /**
   * Delete a specific revision from the database.
   */
  function delete_revision() {
    parent::delete_revision();
  }

  /**
   * Pass values to the provision backend when we call provision-save.
   */
  public function context_options($task_type, $ref_type, &$task) {
    parent::context_options($task_type, $ref_type, $task);

    $task->context_options['letsencrypt_field'] = $this->letsencrypt_field;
  }
}
