/*
The MIT License (MIT)

This file is part of the Phoenard Arduino library
Copyright (c) 2014 Phoenard

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "PHNWidget.h"
#include "PHNDisplay.h"

PHN_Widget::PHN_Widget() {
  this->x = 0;
  this->y = 0;
  this->width = 64;
  this->height = 64;
  this->invalidated = true;
  this->visible = 0x1;
  this->colors = WIDGET_DEFAULT_COLORS;
}

PHN_Widget::~PHN_Widget() {
  // Clear any child widgets in the destructor...
  clearSilent();
  // Make sure to de-register this widget from the Display
  display.removeWidget(*this);
}

void PHN_Widget::setBounds(int x, int y, int width, int height) {
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
  invalidate();
}

void PHN_Widget::setSize(int width, int height) {
  this->width = width;
  this->height = height;
  invalidate();
}

void PHN_Widget::undraw() {
  display.fillRect(x, y, width, height, color(BACKGROUND));
}

void PHN_Widget::setVisible(bool visible) {
  if (isVisible() != visible) {
    if (visible) {
      this->visible |= 0x1;
    } else {
      this->visible &= ~0x1;
    }
    invalidate();
  }
}

void PHN_Widget::setDrawingEnabled(bool drawing) {
  if (drawing) {
    visible &= ~0x4;
  } else {
    visible |= 0x4;
  }
}

bool PHN_Widget::isDrawingEnabled() {
  return (visible & 0x4) != 0x4;
}

bool PHN_Widget::isVisible() {
  return (visible & 0x1) == 0x1;
}

bool PHN_Widget::isDrawn() {
  return (visible & 0x2) == 0x2;
}

void PHN_Widget::setColor(int colorId, color_t color) {
  colors.set(colorId, color);
  for (int i = 0; i < widgetCount(); i++) {
    widget(i)->setColor(colorId, color);
  }
  invalidate();
}

bool PHN_Widget::isTouched() {
  return isVisible() && display.isTouched(x, y, width, height);
}
bool PHN_Widget::isTouchEnter() {
  return isVisible() && display.isTouchEnter(x, y, width, height);
}
bool PHN_Widget::isTouchLeave() {
  return isVisible() && display.isTouchLeave(x, y, width, height);
}
bool PHN_Widget::isClicked() {
  return isVisible() && display.isTouchClicked(x, y, width, height);
}
bool PHN_Widget::isTouchChange() {
  return isVisible() && display.isTouchChange(x, y, width, height);
}
  
void PHN_Widget::invalidate() {
  invalidated = true;
}

bool PHN_Widget::isInvalidated() {
  return invalidated;
}

void PHN_Widget::draw_validate() {
  if ((visible & 0x4) != 0x4) {
    if (visible & 0x1) {
      visible |= 0x2;
      draw();
    } else if (visible & 0x2) {
      visible &= ~0x2;
      undraw();
    }
    invalidated = false;
  }
}

PHN_WidgetContainer::PHN_WidgetContainer() {
  // Note: Initialization below is redundant - left out to save code size
  // Default initializer sets all variables/pointers to 0 already
  // widget_values and widget_removed_values is allowed to be NULL
  // The pointer would never be used, since the values COUNT is 0
  // The initializer is kept because, for some reason, it saves 2 bytes code size (display inheritance)
  widget_count = 0;
  widget_values = NULL;
  deleteAddedWidgets = false;
}

/*
// Note: this adds a lot of codesize because the display destructor would 'use' it
// display is/should never be destructed, but still, it compiles
// all classes that use the widget container that can be destructed: make sure
// to call the clearSilent() function when you do destruct
PHN_WidgetContainer::~PHNWidgetContainer() {
  clearSilent();
}
*/

void PHN_WidgetContainer::updateWidgets(bool update, bool draw, bool forceRedraw) {
  // Update all widgets
  if (update) {
    for (int i = 0; i < widget_count; i++) {
      PHN_Widget *w = widget_values[i];
      if (w->isVisible()) {
        w->updateWidgets(true, false, false);
        w->update();
      }
    }
  }
  // Draw the visible widgets
  if (draw) {
    for (int i = 0; i < widget_count; i++) {
      PHN_Widget *w = widget_values[i];
      bool invalidated = forceRedraw || w->isInvalidated();

      // Redraw all child widgets that are invalidated, first
      w->updateWidgets(false, true, false);

      // Then redraw the main widget and all (visible) child widgets again if invalidated
      if (invalidated) {
        w->draw_validate();
        if (w->isVisible()) {
          w->updateWidgets(false, true, true);
        }
      }
    }
  }
}

void PHN_WidgetContainer::addWidget(PHN_Widget &widget) {
  setWidgetCapacity(widget_count + 1);
  widget_values[widget_count - 1] = &widget;
}

void PHN_WidgetContainer::removeWidget(PHN_Widget &widget) {
  for (int i = 0; i < widget_count; i++) {
    if (widget_values[i] == &widget) {      
      // Move widget to remove to the end of the list
      if (i < (widget_count-1)) {
        for (int d = i; d < (widget_count-1); d++) {
          widget_values[d] = widget_values[d+1];
        }
      }
      widget_values[widget_count - 1] = &widget;

      // Resize array to delete the last element
      setWidgetCapacity(widget_count - 1);
    }
  }
}

void PHN_WidgetContainer::clearSilent() {
  if (deleteAddedWidgets) {
    for (int i = 0; i < widget_count; i++)
      delete widget_values[i];
  }
  delete[] widget_values;
  widget_count = 0;
}

void PHN_WidgetContainer::clearWidgets() {
  setWidgetCapacity(0);
}

void PHN_WidgetContainer::setWidgetCapacity(int capacity) {
  // Allocate a new copy with all widgets within capacity limit copied
  int retainedSize = min(capacity, widget_count) * sizeof(PHN_Widget*);
  PHN_Widget** newValues = new PHN_Widget*[capacity];
  memcpy(newValues, widget_values, retainedSize);

  // Properly 'undraw' (and delete) widgets outside the capacity range
  for (int i = capacity; i < widget_count; i++) {
      PHN_Widget *w = widget_values[i];
      if ((w->visible & 0x3) && !(w->visible & 0x4)) w->undraw();
      if (deleteAddedWidgets) delete w;
  }

  // Delete the old widget values
  delete[] widget_values;

  // Copy over the new resized array
  widget_count = capacity;
  widget_values = newValues;
}