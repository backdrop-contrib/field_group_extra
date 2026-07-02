<?php
/**
 * @file
 * Defines some field group types.
 */

/**
 * Implements field_group_pre_render_<format-type>.
 * Format type: Accordion.
 *
 * @param array $element
 *   The field group form element.
 * @param object $group
 *   The Field group object prepared for pre_render.
 * @param array $form
 *   The root element or form.
 */
function field_group_pre_render_accordion(&$element, $group, &$form) {
  // Add the jQuery UI accordion.
  $element['#attached']['library'][] = array('system', 'ui.accordion');

  $id = !empty($element['#id']) ? ' id="' . $element['#id'] . '"' : '';

  $element += array(
    '#type' => 'markup',
    '#prefix' => '<div class="' . $group->classes . '"' . $id .'>',
    '#suffix' => '</div>',
  );
}

/**
 * Implements field_group_pre_render_<format-type>.
 * Format type: Accordion item.
 *
 * @param array $element
 *   The field group form element.
 * @param object $group
 *   The Field group object prepared for pre_render.
 * @param array $form
 *   The root element or form.
 */
function field_group_pre_render_accordion_item(&$element, $group, &$form) {
  $element += array(
    '#type' => 'markup',
    '#prefix' => '<h3 class="field-group-format-toggler ' . $group->format_type . ($group->collapsed ? '' : ' field-group-accordion-active') . '"><a href="#">' . check_plain(t($group->label)) . '</a></h3>
    <div class="field-group-format-wrapper ' . $group->classes . '">',
    '#suffix' => '</div>',
    //'#attributes' => array('class' => array($group->format_type)),
  );
  if (!empty($group->description)) {
    $element['#prefix'] .= '<div class="description">' . $group->description . '</div>';
  }

}

/**
 * Implements field_group_pre_render_<format-type>.
 * Format type: Horizontal tabs group.
 *
 * @param array $element
 *   The field group form element.
 * @param object $group
 *   The Field group object prepared for pre_render.
 * @param array $form
 *   The root element or form.
 */
function field_group_pre_render_htabs(&$element, $group, &$form) {
  $classes = 'field-group-' . $group->format_type . '-wrapper';
  if (!empty($group->classes)) {
    $classes .= ' ' . $group->classes;
  }

  $id = !empty($element['#id']) ? ' id="' . $element['#id'] . '"' : '';

  $element += array(
    '#type' => 'horizontal_tabs',
    '#title' => check_plain(t($group->label)),
    '#theme_wrappers' => array('horizontal_tabs'),
    '#prefix' => '<div class="' . $classes . '"' . $id . '>',
    '#suffix' => '</div>',
  );

  // By default vertical_tabs don't have titles but you can override it in the theme.
  if (!empty($group->label)) {
    $element['#title'] = check_plain($group->label);
  }

  // Only add form.js on forms.
  if (!empty($form['#type']) && $form['#type'] == 'form') {
    $element['#attached']['library'][] = array('system', 'backdrop.form');
  }

  $element['#attached']['library'][] = array('field_group_extra', 'horizontal-tabs');
}

/**
 * Implements field_group_pre_render_<format-type>.
 * Format type: Horizontal tab.
 *
 * @param array $element
 *   The field group form element.
 * @param object $group
 *   The Field group object prepared for pre_render.
 * @param array $form
 *   The root element or form.
 */
function field_group_pre_render_htab(&$element, $group, &$form) {
  $element += array(
    '#type' => 'fieldset',
    '#title' => check_plain(t($group->label)),
    '#collapsible' => $group->collapsible,
    '#collapsed' => $group->collapsed,
    '#attributes' => array('class' => explode(" ", $group->classes)),
    '#group' => $group->parent_name,
    // very important. Cannot be added on the form!
    '#parents' => array($group->parent_name),
    '#description' => $group->description,
  );

}

/**
 * Implements field_group_pre_render_<format-type>.
 * Format type: HTML element.
 *
 * @param array $element
 *   The field group form element.
 * @param object $group
 *   The Field group object prepared for pre_render.
 * @param array $form
 *   The root element or form.
 */
function field_group_pre_render_html_element(array &$element, $group, array &$form) {
  $html_element = isset($group->format_settings['instance_settings']['wrapper_other']) ? $group->format_settings['instance_settings']['wrapper_other'] : 'div';
  $wrapper = $group->format_settings['instance_settings']['wrapper'];
  if (!empty($wrapper) && $wrapper != 'other') {
    $html_element = $wrapper;
  }
  $show_label = isset($group->format_settings['instance_settings']['show_label']) ? $group->format_settings['instance_settings']['show_label'] : 0;
  $label_element = isset($group->format_settings['instance_settings']['label_element']) ? $group->format_settings['instance_settings']['label_element'] : 'div';
  $configured_attributes = isset($group->format_settings['instance_settings']['attributes']) ? ' ' . $group->format_settings['instance_settings']['attributes'] : '';
  $group->classes = trim($group->classes);

  // This regex split the attributes string so that we can pass that
  // later to backdrop_attributes().
  preg_match_all('/([^\s=]+)="([^"]+)"/', $configured_attributes, $matches);

  $element_attributes = array();
  // Put the attribute and the value together.
  foreach ($matches[1] as $key => $attribute) {
    $element_attributes[$attribute] = $matches[2][$key];
  }

  // Add the classes to the attributes array.
  if (!isset($element_attributes['class']) && $group->classes) {
    $element_attributes['class'] = $group->classes;
  } elseif (isset($element_attributes['class']) && $group->classes) {
    $element_attributes['class'] .= ' ' . $group->classes;
  }

  if (isset($element['#id'])) {
    $element_attributes['id'] = $element['#id'];
  }

  // Sanitize the attributes.
  if (function_exists('filter_xss_attributes')) {
    // Function introduced in 1.30.x.
    $attributes = backdrop_attributes(filter_xss_attributes($element_attributes));
  } else {
    // Assume using core < 1.30.x.
    $element_attributes = _filter_xss_attributes(backdrop_attributes($element_attributes));
    $attributes = $element_attributes ? ' ' . implode(' ', $element_attributes) : '';
  }

  $element['#prefix'] = '<' . $html_element . $attributes . '>';
  if ($show_label) {
    $element['#prefix'] .= '<' . $label_element . '><span>' . check_plain(t($group->label)) . '</span></' . $label_element . '>';
  }
  $element['#suffix'] = '</' . $html_element . '>';
}

/**
 * Implements field_group_pre_render_<format-type>.
 * Format type: Multipage group.
 *
 * @param array $element
 *   The field group form element.
 * @param object $group
 *   The Field group object prepared for pre_render.
 * @param array $form
 *   The root element or form.
 */
function field_group_pre_render_multipage_group(&$element, &$group, &$form) {
  $multipage_element = array(
    '#type' => 'multipage',
    '#theme_wrappers' => array('multipage'),
    '#prefix' => '<div class="field-group-' . $group->format_type . '-wrapper ' . $group->classes . '">',
    '#suffix' => '</div>',
  );

  $element += $multipage_element;

  $move_additional = isset($group->format_settings['instance_settings']['move_additional']) ? ($group->format_settings['instance_settings']['move_additional'] && isset($form['additional_settings'])) : isset($form['additional_settings']);
  $move_button = isset($group->format_settings['instance_settings']['move_button']) ? $group->format_settings['instance_settings']['move_button'] : 0;

  backdrop_add_js(array(
    'field_group' => array(
      'multipage_move_submit' => (bool) $move_button,
      'multipage_move_additional' => (bool) $move_additional
    )
  ), 'setting');

}

/**
 * Implements field_group_pre_render_<format-type>.
 * Format type: Multipage.
 *
 * @param array $element
 *   The field group form element.
 * @param object $group
 *   The Field group object prepared for pre_render.
 * @param array $form
 *   The root element or form.
 */
function field_group_pre_render_multipage(&$element, $group, &$form) {
  $group->classes .= $group->format_settings['formatter'] == 'start' ? ' multipage-open' : ' multipage-closed';
  $element += array(
    '#type' => 'multipage_pane',
    '#title' => check_plain(t($group->label)),
    '#collapsible' => $group->collapsible,
    '#collapsed' => $group->collapsed,
    '#attributes' => array('class' => explode(" ", $group->classes)),
    '#group' => $group->parent_name,
    '#group_object' => $group,
    '#parent_group_object' => $form['#groups'][$group->parent_name],
    // very important. Cannot be added on the form!
    '#parents' => array($group->parent_name),
    '#description' => $group->description,
  );

  $element['#attached']['library'][] = array('field_group_extra', 'multipage');
}

/**
 * Creates a group formatted as horizontal tabs.
 * This function will never be callable from within field_group rendering. Other
 * modules using #type horizontal_tabs will have the benefit of this processor.
 *
 * @param array $element
 *   An associative array containing the properties and children of the
 *   fieldset.
 * @param array $form_state
 *   The $form_state array for the form this horizontal tab widget belongs to.
 *
 * @return array
 *   The processed element.
 */
function form_process_horizontal_tabs($element, &$form_state) {
  // Inject a new fieldset as child, so that form_process_fieldset() processes
  // this fieldset like any other fieldset.
  $element['group'] = array(
    '#type' => 'fieldset',
    '#theme_wrappers' => array(),
    '#parents' => $element['#parents'],
  );

  // The JavaScript stores the currently selected tab in this hidden
  // field so that the active tab can be restored the next time the
  // form is rendered, e.g. on preview pages or when form validation
  // fails.
  $name = implode('__', $element['#parents']);
  if (isset($form_state['values'][$name . '__active_tab'])) {
    $element['#default_tab'] = $form_state['values'][$name . '__active_tab'];
  }
  $element[$name . '__active_tab'] = array(
    '#type' => 'hidden',
    '#default_value' => $element['#default_tab'],
    '#attributes' => array('class' => array('horizontal-tabs-active-tab')),
  );

  return $element;
}

/**
 * Creates a group formatted as multipage.
 * This function will never be callable from within field_group rendering. Other
 * modules using #type multipage will have the benefit of this processor.
 *
 * @param array $element
 *   An associative array containing the properties and children of the
 *   fieldset.
 * @param array $form_state
 *   The $form_state array for the form this multipage tab widget belongs to.
 *
 * @return array
 *   The processed element.
 */
function form_process_multipage($element, &$form_state) {
  // Inject a new fieldset as child, so that form_process_fieldset() processes
  // this fieldset like any other fieldset.
  $element['group'] = array(
    '#type' => 'fieldset',
    '#theme_wrappers' => array(),
    '#parents' => $element['#parents'],
  );

  // The JavaScript stores the currently selected tab in this hidden
  // field so that the active control can be restored the next time the
  // form is rendered, e.g. on preview pages or when form validation
  // fails.
  $name = implode('__', $element['#parents']);
  if (isset($form_state['values'][$name . '__active_control'])) {
    $element['#default_tab'] = $form_state['values'][$name . '__active_control'];
  }
  $element[$name . '__active_control'] = array(
    '#type' => 'hidden',
    '#default_value' => $element['#default_control'],
    '#attributes' => array('class' => array('multipage-active-control')),
  );

  return $element;
}
