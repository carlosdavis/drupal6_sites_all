// $Id: markitup.js,v 1.1 2009/01/06 01:40:31 sun Exp $

/**
 * Attach this editor to a target element.
 */
Drupal.wysiwyg.editor.attach.markitup = function(context, params, settings) {
  $('#' + params.field, context).markItUp(settings);
};

/**
 * Detach a single or all editors.
 *
 * See Drupal.wysiwyg.editor.detach.none() for a full desciption of this hook.
 */
Drupal.wysiwyg.editor.detach.markitup = function(context, params) {
  if (typeof params != 'undefined') {
    $('#' + params.field, context).markItUpRemove();
  }
  else {
    $('.markItUpEditor', context).markItUpRemove();
  }
};

