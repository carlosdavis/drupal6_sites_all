<?php
// $Id: whizzywig.inc,v 1.1 2008/11/17 00:06:33 sun Exp $


/**
 * Plugin implementation of hook_editor().
 */
function wysiwyg_whizzywig_editor() {
  $editor = array();
  $editor['whizzywig'] = array(
    'title' => 'Whizzywig',
    'vendor url' => 'http://www.unverse.net',
    'download url' => 'http://www.unverse.net/whizzywig-download.html',
    'library path' => wysiwyg_get_path('whizzywig'),
    'libraries' => array(
      '' => array(
        'title' => 'Default',
        'files' => array('whizzywig.js', 'xhtml.js'),
      ),
    ),
    'version callback' => 'wysiwyg_whizzywig_version',
    'settings callback' => 'wysiwyg_whizzywig_settings',
    'plugin callback' => 'wysiwyg_whizzywig_plugins',
    'versions' => array(
      '55' => array(
        'js files' => array('whizzywig.js'),
      ),
    ),
  );
  return $editor;
}

/**
 * Detect editor version.
 *
 * @param $editor
 *   An array containing editor properties as returned from hook_editor().
 *
 * @return
 *   The installed editor version.
 */
function wysiwyg_whizzywig_version($editor) {
  $script = wysiwyg_get_path('whizzywig') . '/whizzywig.js';
  $script = fopen($script, 'r');
  $line = fgets($script, 43);
  if (preg_match('@Whizzywig v([0-9]+)@', $line, $version)) {
    fclose($script);
    return $version[1];
  }
}

/**
 * Return runtime editor settings for a given wysiwyg profile.
 *
 * @param $editor
 *   A processed hook_editor() array of editor properties.
 * @param $config
 *   An array containing wysiwyg editor profile settings.
 * @param $theme
 *   The name of a theme/GUI/skin to use.
 *
 * @return
 *   A settings array to be populated in
 *   Drupal.settings.wysiwyg.configs.{editor}
 */
function wysiwyg_whizzywig_settings($editor, $config, $theme) {
  $settings = array();

  // Add path to button images, if available.
  if (is_dir(wysiwyg_get_path('whizzywig') . '/btn')) {
    $settings['buttonPath'] = wysiwyg_get_path('whizzywig', TRUE) . '/btn/';
  }

  // Add configured buttons or all available.
  if (!empty($config['buttons'])) {
    $buttons = array();
    foreach ($config['buttons'] as $plugin) {
      $buttons = array_merge($buttons, $plugin);
    }
    $settings['buttons'] = implode(' ', array_keys($buttons));
  }
  else {
    $settings['buttons'] = 'all';
  }

  // Add editor content stylesheet.
  if (isset($config['css_setting'])) {
    if ($config['css_setting'] == 'theme') {
      $css = path_to_theme() .'/style.css';
      if (file_exists($css)) {
        $settings['externalCSS'] = base_path() . $css;
      }
    }
    else if ($config['css_setting'] == 'self' && isset($config['css_path'])) {
      $settings['externalCSS'] = strtr($config['css_path'], array('%b' => base_path(), '%t' => path_to_theme()));
    }
  }

  return $settings;
}

/**
 * Return internal plugins for Whizzywig; semi-implementation of hook_wysiwyg_plugin().
 */
function wysiwyg_whizzywig_plugins($editor) {
  return array(
    'default' => array(
      'buttons' => array(
        'formatblock' => t('HTML block format'), 'fontname' => t('Font'), 'fontsize' => t('Font size'),
        'bold' => t('Bold'), 'italic' => t('Italic'), 'underline' => t('Underline'),
        'left' => t('Align left'), 'center' => t('Align center'), 'right' => t('Align right'),
        'bullet' => t('Bullet list'), 'number' => t('Numbered list'),
        'outdent' => t('Outdent'), 'indent' => t('Indent'),
        'undo' => t('Undo'), 'redo' => t('Redo'),
        'image' => t('Image'),
        'color' => t('Forecolor'), 'hilite' => t('Backcolor'),
        'rule' => t('Horizontal rule'),
        'link' => t('Link'),
        'image' => t('Image'),
        'table' => t('Table'),
        'clean' => t('Clean-up'),
        'html' => t('Source code'),
        'spellcheck' => t('Spell check'),
      ),
      'internal' => TRUE,
    ),
  );
}

