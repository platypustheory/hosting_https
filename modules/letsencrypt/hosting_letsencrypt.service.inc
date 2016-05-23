<?php

/**
 * @file
 * Letsencrypt service implementation of the Certificate service type for the hosting front end.
 */

/**
 * An implementation of the certificate service type, registered with hook_hosting_service.
 */

class hostingService_Certificate_Letsencrypt extends hostingService_Certificate {
  /**
   * the value stored in the type column of the hosting_service table.
   */
  public $type = 'Certificate';

  protected $certificate_field = 'default';

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
    $this->certificate_field = variable_get('hosting_certificate_field_' . $this->server->nid, $this->certificate_field);

    /**
     * Although this certificate does not have it's own tables, we provide some utitilty functions
     * for use in this method.
     *
     * If this certificate used it's own tables, we could use the mergeData method below to merge in the
     * results automatically, instead of iterating through the results ourself.
     */

    // $this->mergeData("SELECT certificate_field FROM {hosting_certificate} WHERE vid = :vid", array(':vid' => $this->server->vid));
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
    $render['certificate_field'] = array(
      '#type' => 'item',
      '#title' => t('Certificate field'),
      // Remember to pass the display through filter_xss!
      '#markup' => filter_xss($this->certificate_field),
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

    $form['certificate_field'] = array(
      '#type' => 'textfield',
      '#title' => t('Certificate field'),
      '#description' => t('An certificate field for the user to fill in.'),
      '#size' => 40,
      '#default_value' => $this->certificate_field,
      '#maxlength' => 64,
      '#weight' => 5,
    );
  }

  /**
   * Validate a form submission.
   */
  function validate(&$node, &$form, &$form_state) {
    parent::validate($node, $form, $form_state);

    if (sizeof($this->certificate_field) > 30) {
      form_set_error('certificate_field', t("The certificate string must not be longer than 30 characters"));
    }
  }

  /**
   * Insert a record into the database.
   */
  function insert() {
    parent::insert();

    variable_set('hosting_certificate_field_' . $this->server->nid, $this->certificate_field);
  }

  /**
   * Update a record in the database.
   */
  function update() {
    parent::update();

    variable_set('hosting_certificate_field_' . $this->server->nid, $this->certificate_field);
  }

  /**
   * Delete a record from the database, based on server node.
   */
  function delete() {
    parent::delete();

    variable_del('hosting_certificate_field_' . $this->server->nid);
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

    $task->context_options['certificate_field'] = $this->certificate_field;
  }
}
