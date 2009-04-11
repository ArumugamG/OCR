/**
 * @file TextPage.h
 * @author Corey Ford <fordco@sonoma.edu>
 * @date Spring 2009
 * @brief The interface and documentation of the TextPage class.
 * @see TextPage.cpp for the implementation.
 */
/*
 * Copyright 2009 Corey Ford
 *
 * This file is part of Omophagic Content Reader.
 *
 * Omophagic Content Reader is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Omophagic Content Reader is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Omophagic Content Reader.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef TEXTPAGE_H_
#define TEXTPAGE_H_
#include <queue>
#include <vector>
#include <string>
#include "EasyBMP/EasyBMP.h"
#include "TextLine.h"

namespace OCR
{
class TextPage
{
public:
	/**
	 * Initializes the class with a BMP image
	 */
	TextPage(BMP & img);

	/**
	 * Processes the image.
	 * @return text of the image.
	 */
	std::string Read ();

private:
	/**
	 * Represents the position
	 * of a pixel in the image
	 */
	struct Pixel
	{
		Pixel (int left, int top) : x (left), y(top) {}
		bool operator!= (const Pixel & rhs) const
		{ return this->x != rhs.x || this->y != rhs.y; }
		int x, y;
	};

	/**
	 * Represents a rectangle,
	 * such as the extent of a letter
	 */
	struct Box
	{
		Box (int x1, int y1, int x2, int y2) :
			low (x1, y1), high (x2, y2) {}
		Pixel low, high;
	};

	/**
	 * @param[in] point Pixel to check
	 * @return whether the specified Pixel is a foreground pixel.
	 */
	bool isForeground (Pixel point);

	/**
	 * Returns whether the specified Pixel
	 * has been marked as visited, and marks
	 * it as visited if it is not.
	 * @param[in] p Pixel to check
	 * @return whether this Pixel has been visited already.
	 */
	bool isVisited (Pixel p);

	/**
	 * @param[in] p Pixel to find neighbors of
	 * @return the next neighbor of the specified Pixel
	 */
	Pixel nextNeighbor (Pixel p);

	/**
	 * Finds the extent of a contiguous shape, starting at the specified Pixel.
	 * @param[in] start Pixel to start at
	 * @return a Box representing the extent of the letter
	 */
	Box findContiguousShape (Pixel start);

	/// the image
	BMP & image;
	/// the lines of text
	std::vector<TextLine> lines;
	/// array indicating which pixels have been visited
	bool ** visited;
};

}; // namespace GraphemeResolver
#endif /*TEXTPAGE_H_*/
