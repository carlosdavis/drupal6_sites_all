<?php
// $Id: fckeditor.inc,v 1.12 2009/03/21 23:16:15 sun Exp $

/**
 * @file
 * Editor integration functions for FCKeditor.
 */


/**
 * Plugin implementation of hook_editor().
 */
function wysiwyg_fckeditor_editor() {
  $editor = array();
  $editor['fckeditor'] = array(
    'title' => 'FCKeditor',
    'vendor url' => 'http://www.fckeditor.net',
    'download url' => 'http://www.fckeditor.net/download',
    'library path' => wysiwyg_get_path('fckeditor'),
    'libraries' => array(
      '' => array(
        'title' => 'Default',
        'files' => array('fckeditor.js'),
      ),
    ),
    'version callback' => 'wysiwyg_fckeditor_version',
    'themes callback' => 'wysiwyg_fckeditor_themes',
    'settings callback' => 'wysiwyg_fckeditor_settings',
    'plugin callback' => 'wysiwyg_fckeditor_plugins',
    'plugin settings callback' => 'wysiwyg_fckeditor_plugin_settings',
    'proxy plugin' => array(
      'drupal' => array(
        'load' => TRUE,
        'proxy' => TRUE,
      ),
    ),
    'proxy plugin settings callback' => 'wysiwyg_fckeditor_proxy_plugin_settings',
    'versions' => array(
      2.6 => array(
        'js files' => array('fckeditor-2.6.js'),
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
function wysiwyg_fckeditor_version($editor) {
  $library = $editor['library path'] .'/fckeditor.js';
  $library = fopen($library, 'r');
  $max_lines = 100;
  while ($max_lines && $line = fgets($library, 60)) {
    if (preg_match('@^FCKeditor.prototype.Version\s*= \'([\d\.]+)@', $line, $version)) {
      fclose($library);
      return $version[1];
    }
    $max_lines--;
  }
  fclose($library);
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
function wysiwyg_fckeditor_settings($editor, $config, $theme) {
  $settings = array(
    'EditorPath' => base_path() . $editor['library path'] .'/',
    'SkinPath' => base_path() . $editor['library path'] .'/editor/skins/'. $theme .'/',
    'Width' => '100%',
    'Height' => 420,
    'LinkBrowser' => FALSE,
    'LinkUpload' => FALSE,
    'ImageBrowser' => FALSE,
    'ImageUpload' => FALSE,
    'FlashBrowser' => FALSE,
    'FlashUpload' => FALSE,
  );
  if (isset($config['block_formats'])) {
    $settings['FontFormats'] = strtr($config['block_formats'], array(',' => ';'));
  }
  if (isset($config['apply_source_formatting'])) {
    $settings['FormatSource'] = $config['apply_source_formatting'];
  }
  if (isset($config['preformatted'])) {
    $settings['FormatOutput'] = $config['preformatted'];
  }

  if (isset($config['css_setting'])) {
    if ($config['css_setting'] == 'theme') {
      $settings['EditorAreaCSS'] = implode(',', wysiwyg_get_css());
    }
    else if ($config['css_setting'] == 'self' && isset($config['css_path'])) {
      $settings['EditorAreaCSS'] = strtr($config['css_path'], array('%b' => base_path(), '%t' => path_to_theme()));
    }
  }

  if (!empty($config['buttons'])) {
    // Use our custom toolbar set.
    $settings['CustomConfigurationsPath'] = wysiwyg_get_path('editors/js/fckeditor.config.js', TRUE);
    $settings['ToolbarSet'] = 'Wysiwyg';
    // Populate our custom toolbar set for fckeditor.config.js.
    $settings['buttons'] = array();
    $plugins = wysiwyg_get_plugins($editor['name']);
    foreach ($config['buttons'] as $plugin => $buttons) {
      foreach ($buttons as $button => $enabled) {
        // Iterate separately over buttons and extensions properties.
        foreach (array('buttons', 'extensions') as $type) {
          // Skip unavailable plugins.
          if (!isset($plugins[$plugin][$type][$button])) {
            continue;
          }
          // Add buttons.
          if ($type == 'buttons') {
            $settings['buttons'][] = $button;
          }
          // Allow plugins to add or override global configuration settings.
          if (!empty($plugins[$plugin]['options'])) {
            $settings = array_merge($settings, $plugins[$plugin]['options']);
          }
        }
      }
    }
    // For now, all buttons are placed into one row.
    if (!empty($settings['buttons'])) {
      $settings['buttons'] = array($settings['buttons']);
    }
  }

  return $settings;
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
function wysiwyg_fckeditor_themes($editor, $profile) {
  return array('default', 'office2003', 'silver');
}

/**
 * Build a JS settings array of native external plugins that need to be loaded separately.
 *
 * @todo Required for FCKeditor?  If not, wysiwyg_add_plugin_settings() needs
 *   an overhaul.
 */
function wysiwyg_fckeditor_plugin_settings($editor, $profile, $plugins) {
  $settings = array();
  return $settings;
}

/**
 * Build a JS settings array for Drupal plugins loaded via the proxy plugin.
 */
function wysiwyg_fckeditor_proxy_plugin_settings($editor, $profile, $plugins) {
  $settings = array();
  foreach ($plugins as $name => $plugin) {
    // Populate required plugin settings.
    $settings[$name] = $plugin['dialog settings'] + array(
      'title' => $plugin['title'],
      'icon' => base_path() . $plugin['icon path'] .'/'. $plugin['icon file'],
      'iconTitle' => $plugin['icon title'],
      // @todo These should only be set if the plugin defined them.
      'css' => base_path() . $plugin['css path'] .'/'. $plugin['css file'],
    );
  }
  return $settings;
}

/**
 * Return internal plugins for FCKeditor; semi-implementation of hook_wysiwyg_plugin().
 */
function wysiwyg_fckeditor_plugins($editor) {
  return array(
    'default' => array(
      'buttons' => array(
        'Bold' => t('Bold'), 'Italic' => t('Italic'), 'Underline' => t('Underline'),
        'StrikeThrough' => t('Strike-through'),
        'JustifyLeft' => t('Align left'), 'JustifyCenter' => t('Align center'), 'JustifyRight' => t('Align right'), 'JustifyFull' => t('Justify'),
        'UnorderedList' => t('Bullet list'), 'OrderedList' => t('Numbered list'),
        'Outdent' => t('Outdent'), 'Indent' => t('Indent'),
        'Undo' => t('Undo'), 'Redo' => t('Redo'),
        'Link' => t('Link'), 'Unlink' => t('Unlink'), 'Anchor' => t('Anchor'),
        'Image' => t('Image'),
        'TextColor' => t('Forecolor'), 'BGColor' => t('Backcolor'),
        'Superscript' => t('Superscript'), 'Subscript' => t('Subscript'),
        'Blockquote' => t('Blockquote'), 'Source' => t('Source code'),
        'Rule' => t('Horizontal rule'),
        'Cut' => t('Cut'), 'Copy' => t('Copy'), 'Paste' => t('Paste'),
        'PasteText' => t('Paste Text'), 'PasteWord' => t('Paste from Word'),
        'ShowBlocks' => t('Show blocks'),
        'RemoveFormat' => t('Remove format'),
        'SpecialChar' => t('Character map'),
        'About' => t('About'),

        'FontFormat' => t('HTML block format'), 'FontName' => t('Font'), 'FontSize' => t('Font size'), 'Style' => t('Font style'),
        'Table' => t('Table'),
        'Find' => t('Search'), 'Replace' => t('Replace'), 'SelectAll' => t('Select all'),
        'CreateDiv' => t('Create DIV container'),
        'Flash' => t('Flash'), 'Smiley' => t('Smiley'),
        'FitWindow' => t('FitWindow'),
        'SpellCheck' => t('Check spelling'),
      ),
      'internal' => TRUE,
    ),
  );
}

