<?php

/**
 * @file
 * Certificate service type for the hosting front end.
 */

/**
 * The base service type class , registered with hook_hosting_service_type.
 */
class hostingService_Certificate extends hostingService {
  /**
   * the value stored in the service column of hosting_service table.
   */
  public $service = 'Certificate';
}

