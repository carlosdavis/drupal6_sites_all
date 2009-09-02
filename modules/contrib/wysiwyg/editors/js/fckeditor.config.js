// $Id: fckeditor.config.js,v 1.4 2009/03/26 22:31:42 sun Exp $

Drupal = window.parent.Drupal;

/**
 * Fetch and provide original editor settings as local variable.
 *
 * FCKeditor does not support to pass complex variable types to the editor.
 * Instance settings passed to FCKinstance.Config are temporarily stored in
 * FCKConfig.PageConfig.
 */
var wysiwygFormat = FCKConfig.PageConfig.wysiwygFormat;
var wysiwygSettings = Drupal.settings.wysiwyg.configs.fckeditor[wysiwygFormat];

/**
 * Apply format-specific settings.
 */
for (var setting in wysiwygSettings) {
  if (setting == 'buttons') {
    // Apply custom Wysiwyg toolbar for this format.
    FCKConfig.ToolbarSets['Wysiwyg'] = wysiwygSettings.buttons;
  }
  else {
    FCKConfig[setting] = wysiwygSettings[setting];
  }
}

/**
 * Initialize this editor instance.
 */
Drupal.wysiwyg.editor.instance.fckeditor.init(window);

/**
 * Register Drupal plugins for this input format.
 *
 * Parameters to addPlugin() are:
 * - Plugin name.
 * - Format specific plugin settings.
 * - General plugin settings.
 * - A reference to this window so the plugin setup can access FCKConfig.
 */
for (var plugin in Drupal.settings.wysiwyg.plugins[wysiwygFormat].drupal) {
  Drupal.wysiwyg.editor.instance.fckeditor.addPlugin(plugin, Drupal.settings.wysiwyg.plugins[wysiwygFormat].drupal[plugin], Drupal.settings.wysiwyg.plugins.drupal[plugin], window);
}

