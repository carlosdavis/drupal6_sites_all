<?php
// $Id: break.inc,v 1.2 2009/02/01 10:06:21 sun Exp $

/**
 * @file
 * Wysiwyg API integration on behalf of Node module.
 */

/**
 * Implementation of hook_wysiwyg_plugin().
 */
function wysiwyg_break_plugin() {
  $plugins = array();
  $plugins['break'] = array(
    'title' => t('Teaser break'),
    'vendor url' => 'http://drupal.org/project/wysiwyg',
    'icon file' => 'break.gif',
    'icon title' => t('Separate the teaser and body of this content'),
    'settings' => array(
      // Path is set by default.
      // 'path' => wysiwyg_get_path('plugins/break', TRUE),
    ),
  );
  return $plugins;
}

