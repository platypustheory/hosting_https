<?php

/**
 * @file
 * Certificate service implementation for the hosting front end.
 *
 * @addtogroup hostingserviceapi
 * @{
 */

/**
 * The base service type class , registered with hook_hosting_service_type.
 */
class hostingService_certificate extends hostingService {
  /**
   * the value stored in the service column of hosting_service table.
   */
  public $service = 'certificate';
}


/**
 * An implementation of the certificate service type, registered with hook_hosting_service.
 */

class hostingService_certificate_selfsigned extends hostingService_certificate {
  /**
   * the value stored in the type column of the hosting_service table.
   */
  public $type = 'selfsigned';


  /**
   * this service needs to have a port specified for it.
   */
  public $has_port = TRUE;

  /**
   * the default value for the port input.
   */
  function default_port() {
    return 12345;
  }

  /**
   * this service needs to be restarted with a shell command.
   */
  public $has_restart_cmd = TRUE;

  /**
   * The default value for the restart command input.
   */
  function default_restart_cmd() {
    return "/usr/bin/true";
  }



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
    $this->certificate_field = variable_get('hosting_certificate_field_' . $this->server->nid, 'default value');

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
   * Modify the reference passed to the method to add additional implementation
   * specific fields to be displayed.
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
   * Modify the reference passed to the method to add additional implementation
   * specific fields to be stored and managed.
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
      '#default_value' => ($this->certificate_field) ? $this->certificate_field : 'default value',
      '#maxlength' => 64,
      '#weight' => 5,
    );
  }

  /**
   * Validate a form submission.
   */
  function validate(&$node, &$form) {
    parent::validate($node, $form);

    if (sizeof($this->certificate_field) > 30) {
      form_set_error('certificate_field', t("The certificate string must not be longer than 30 characters"));
    }
  }

  /**
   * Insert a record into the database.
   *
   * Called by hosting_server_hook_insert().
   *
   * The values associated with this implementation have already
   * been set as properties of $this object, so we now need to
   * save them.
   *
   * For this certificate we will use the variables table, but you should
   * create your own tables with an install file and hook_schema.
   */
  function insert() {
    parent::insert();

    variable_set('hosting_certificate_field_' . $this->server->nid, $this->certificate_field);
  }

  /**
   * Update a record in the database.
   *
   * Called by hosting_server_hook_update().
   *
   * For this certificate we will use the variables table, but you should
   * create your own tables with an install file and hook_schema.
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
   * Delete a specific reivision from the database.
   *
   * Not relevant in our certificate but shown anyway.
   */
  function delete_revision() {
    parent::delete_revision();
  }

  /**
   * Pass values to the provision backend when we call provision-save.
   *
   * By selecting this type we already pass the '--certificate_service_type=selfsigned' option
   * to the command, which will load the matching provisionService class in the backend.
   *
   * This backend class will be responsible for receiving and reacting to the options
   * passed here.
   *
   * @ingroup backend-frontend-IPC
   */
  public function context_options($task_type, $ref_type, &$task) {
    parent::context_options($task_type, $ref_type, $task);

    $task->context_options['certificate_field'] = $this->certificate_field;
  }
}

/**
 * @} End of "addtogroup hostingserviceapi".
 */
