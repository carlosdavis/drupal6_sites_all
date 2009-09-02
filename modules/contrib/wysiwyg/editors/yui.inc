<?php
// $Id: yui.inc,v 1.3 2009/03/04 11:27:18 sun Exp $


/**
 * Plugin implementation of hook_editor().
 */
function wysiwyg_yui_editor() {
  $editor = array();
  $editor['yui'] = array(
    'title' => 'YUI',
    'vendor url' => 'http://developer.yahoo.com/yui/editor/',
    'download url' => 'http://developer.yahoo.com/yui/download/',
    'library path' => wysiwyg_get_path('yui') . '/build',
    'libraries' => array(
      'min' => array(
        'title' => 'Minified',
        'files' => array(
          'yahoo-dom-event/yahoo-dom-event.js',
          'animation/animation-min.js',
          'element/element-min.js',
          'container/container-min.js',
          'menu/menu-min.js',
          'button/button-min.js',
          'editor/editor-min.js',
        ),
      ),
      'src' => array(
        'title' => 'Source',
        'files' => array(
          'yahoo-dom-event/yahoo-dom-event.js',
          'animation/animation.js',
          'element/element.js',
          'container/container.js',
          'menu/menu.js',
          'button/button.js',
          'editor/editor.js',
        ),
      ),
    ),
    'version callback' => 'wysiwyg_yui_version',
    'load callback' => 'wysiwyg_yui_load',
    'settings callback' => 'wysiwyg_yui_settings',
    'themes callback' => 'wysiwyg_yui_themes',
    'plugin callback' => 'wysiwyg_yui_plugins',
    'versions' => array(
      '2.6.0' => array(
        'js files' => array('yui.js'),
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
function wysiwyg_yui_version($editor) {
  $library = $editor['library path'] . '/editor/editor.js';
  $library = fopen($library, 'r');
  $max_lines = 10;
  while ($max_lines && $line = fgets($library, 60)) {
    if (preg_match('@version:\s([0-9\.]+)$@', $line, $version)) {
      fclose($library);
      return $version[1];
    }
    $max_lines--;
  }
  fclose($library);
}

/**
 * Perform additional actions upon loading this editor.
 *
 * @param $editor
 *   A processed hook_editor() array of editor properties.
 * @param $library
 *   The internal library name (array key) to use.
 */
function wysiwyg_yui_load($editor, $library) {
  drupal_add_css($editor['library path'] . '/menu/assets/skins/sam/menu.css');
  drupal_add_css($editor['library path'] . '/button/assets/skins/sam/button.css');
  drupal_add_css($editor['library path'] . '/fonts/fonts-min.css');
  drupal_add_css($editor['library path'] . '/container/assets/skins/sam/container.css');
  drupal_add_css($editor['library path'] . '/editor/assets/skins/sam/editor.css');
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
function wysiwyg_yui_settings($editor, $config, $theme) {
  $settings = array(
    'theme' => $theme,
    'dompath' => $config['path_loc'],
    'animate' => TRUE,
    'handleSubmit' => TRUE,
    'markup' => 'xhtml',
    'ptags' => TRUE,
    'toolbar' => array(
      'collapse' => FALSE,
      'buttonType' => 'simple',
    ),
  );
  if (isset($config['buttons'])) {
    $buttons = array();
    foreach ($config['buttons'] as $plugin => $enabled_buttons) {
      foreach ($enabled_buttons as $button => $enabled) {
        $extra = array();
        if ($button == 'heading') {
          $extra = array('menu' => array(
            array('text' => 'Paragraph', 'value' => 'p', 'checked' => TRUE),
            array('text' => 'Header 1', 'value' => 'h1'),
            array('text' => 'Header 2', 'value' => 'h2'),
            array('text' => 'Header 3', 'value' => 'h3'),
            array('text' => 'Header 4', 'value' => 'h4'),
            array('text' => 'Header 5', 'value' => 'h5'),
            array('text' => 'Header 6', 'value' => 'h6'),
          ));
        }
        else if ($button == 'fontstyle') {
          $extra = array('menu' => array(
            array('text' => 'Arial', 'checked' => TRUE),
            array('text' => 'Arial Black'),
            array('text' => 'Comic Sans MS'),
            array('text' => 'Courier New'),
            array('text' => 'Lucida Console'),
            array('text' => 'Tahoma'),
            array('text' => 'Times New Roman'),
            array('text' => 'Trebuchet MS'),
            array('text' => 'Verdana'),
          ));
        }
        $buttons[] = wysiwyg_yui_button_setting($editor, $plugin, $button, $extra);
      }
    }
    // Group triggers an JS error for any reason. 06/12/2008 sun
    // $settings['toolbar']['buttons'][] = array('group' => 'default', 'label' => '', 'buttons' => $toolbar);
    // JS error disappears when adding another button, but not when adding a
    // separator. 06/12/2008 sun
    // $settings['toolbar']['buttons'][] = wysiwyg_yui_button_setting($editor, 'default', 'separator');
    $settings['toolbar']['buttons'] = $buttons;
  }

  return $settings;
}

/**
 * Create the JavaScript structure for a YUI button.
 *
 * @param $editor
 *   A processed hook_editor() array of editor properties.
 * @param $plugin
 *   The internal name of a plugin.
 * @param $button
 *   The internal name of a button, defined by $plugin.
 * @param $extra
 *   (optional) An array containing arbitrary other elements to add to the
 *   resulting button.
 */
function wysiwyg_yui_button_setting($editor, $plugin, $button, $extra = array()) {
  static $plugins;
  if (!isset($plugins)) {
    // @todo Invoke all enabled plugins, not just internals.
    $plugins = wysiwyg_yui_plugins($editor);
  }
  // Return a simple separator.
  if ($button == 'separator') {
    return array('type' => 'separator');
  }
  $type = 'push';
  if (in_array($button, array('heading', 'fontname'))) {
    $type = 'select';
  }
  else if (in_array($button, array('fontsize'))) {
    $type = 'spin';
  }
  $button = array(
    'type' => $type,
    'label' => $plugins[$plugin]['buttons'][$button],
    'value' => $button,
  );
  // Add arbitrary other elements, if defined.
  if (!empty($extra)) {
    $button = array_merge($button, $extra);
  }
  return $button;
}

/**
 * Determine available editor themes or check/reset a given one.
 *
 * @param $editor
 *   A processed hook_editor() array of editor properties.
 * @param $profile
 *   A wysiwyg editor profile.
 *
 * @return
 *   An array of theme names. The first returned name should be the default
 *   theme name.
 */
function wysiwyg_yui_themes($editor, $profile) {
  return array('sam');
}

/**
 * Return internal plugins for this editor; semi-implementation of hook_wysiwyg_plugin().
 */
function wysiwyg_yui_plugins($editor) {
  return array(
    'default' => array(
      'buttons' => array(
        'bold' => t('Bold'), 'italic' => t('Italic'), 'underline' => t('Underline'),
        'strikethrough' => t('Strike-through'),
        'justifyleft' => t('Align left'), 'justifycenter' => t('Align center'), 'justifyright' => t('Align right'),
        'insertunorderedlist' => t('Bullet list'), 'insertorderedlist' => t('Numbered list'),
        'outdent' => t('Outdent'), 'indent' => t('Indent'),
        'createlink' => t('Link'),
        'insertimage' => t('Image'),
        'forecolor' => t('Font Color'), 'backcolor' => t('Background Color'), 
        'superscript' => t('Sup'), 'subscript' => t('Sub'),
        'removeformat' => t('Remove format'),
        'hiddenelements' => t('Show/hide hidden elements'),
        'formatselect' => t('HTML block format'), 'fontstyle' => t('Font'), 'fontsize' => t('Font size'), 'styleselect' => t('Font style'),
      ),
      'internal' => TRUE,
    ),
  );
}

