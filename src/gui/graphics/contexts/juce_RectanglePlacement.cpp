/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-10 by Raw Material Software Ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the GNU General
   Public License (Version 2), as published by the Free Software Foundation.
   A copy of the license is included in the JUCE distribution, or can be found
   online at www.gnu.org/licenses.

   JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
   A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  ------------------------------------------------------------------------------

   To release a closed-source product which uses JUCE, commercial licenses are
   available: visit www.rawmaterialsoftware.com/juce for more information.

  ==============================================================================
*/

#include "../../../core/juce_StandardHeader.h"

BEGIN_JUCE_NAMESPACE

#include "juce_RectanglePlacement.h"


//==============================================================================
RectanglePlacement::RectanglePlacement (const RectanglePlacement& other) throw()
    : flags (other.flags)
{
}

RectanglePlacement& RectanglePlacement::operator= (const RectanglePlacement& other) throw()
{
    flags = other.flags;
    return *this;
}

void RectanglePlacement::applyTo (double& x, double& y,
                                  double& w, double& h,
                                  const double dx, const double dy,
                                  const double dw, const double dh) const throw()
{
    if (w == 0 || h == 0)
        return;

    if ((flags & stretchToFit) != 0)
    {
        x = dx;
        y = dy;
        w = dw;
        h = dh;
    }
    else
    {
        double scale = (flags & fillDestination) != 0 ? jmax (dw / w, dh / h)
                                                      : jmin (dw / w, dh / h);

        if ((flags & onlyReduceInSize) != 0)
            scale = jmin (scale, 1.0);

        if ((flags & onlyIncreaseInSize) != 0)
            scale = jmax (scale, 1.0);

        w *= scale;
        h *= scale;

        if ((flags & xLeft) != 0)
            x = dx;
        else if ((flags & xRight) != 0)
            x = dx + dw - w;
        else
            x = dx + (dw - w) * 0.5;

        if ((flags & yTop) != 0)
            y = dy;
        else if ((flags & yBottom) != 0)
            y = dy + dh - h;
        else
            y = dy + (dh - h) * 0.5;
    }
}

const AffineTransform RectanglePlacement::getTransformToFit (const Rectangle<float>& source, const Rectangle<float>& destination) const throw()
{
    if (source.isEmpty())
        return AffineTransform::identity;

    float w = source.getWidth();
    float h = source.getHeight();

    const float scaleX = destination.getWidth() / w;
    const float scaleY = destination.getHeight() / h;

    if ((flags & stretchToFit) != 0)
        return AffineTransform::translation (-source.getX(), -source.getY())
                    .scaled (scaleX, scaleY)
                    .translated (destination.getX(), destination.getY());

    float scale = (flags & fillDestination) != 0 ? jmax (scaleX, scaleY)
                                                 : jmin (scaleX, scaleY);

    if ((flags & onlyReduceInSize) != 0)
        scale = jmin (scale, 1.0f);

    if ((flags & onlyIncreaseInSize) != 0)
        scale = jmax (scale, 1.0f);

    w *= scale;
    h *= scale;

    float newX = destination.getX();
    float newY = destination.getY();

    if ((flags & xRight) != 0)
        newX += destination.getWidth() - w;             // right
    else if ((flags & xLeft) == 0)
        newX += (destination.getWidth() - w) / 2.0f;    // centre

    if ((flags & yBottom) != 0)
        newY += destination.getHeight() - h;             // bottom
    else if ((flags & yTop) == 0)
        newY += (destination.getHeight() - h) / 2.0f;    // centre

    return AffineTransform::translation (-source.getX(), -source.getY())
                .scaled (scale, scale)
                .translated (newX, newY);
}


END_JUCE_NAMESPACE
