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

/**
 * @file
 * @brief Contains the PHN_Label widget; a text-showing widget
 */

#include "PHNWidget.h"

#ifndef _PHN_LABEL_H_
#define _PHN_LABEL_H_

/**
 * @brief The label widget shows text or numeric data
 *
 * The text is automatically sized centred inside the bounds,
 * with an optional frame around the text set using setDrawFrame().
 */
class PHN_Label : public PHN_Widget, public PHN_TextContainer {
 public:
  /// Changed constructor to invert the colors
  PHN_Label(void);
  /// Sets the text to be displayed
  virtual void setTextRaw(const char* text, int textLen);
  /// Gets the text currently displayed
  virtual const char* text() { return (char*) this->textBuff.data; }
  /// Sets whether a frame border is drawn
  const void setDrawFrame(bool draw) { drawFrame = draw; }
  virtual void update(void);
  virtual void draw(void);
 private:
  DataBuffer textBuff;
  bool drawFrame;
  bool quickDraw;
};

#endif