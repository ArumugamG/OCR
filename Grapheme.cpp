/**
 * @file Grapheme.cpp
 * @date Spring 2009
 * @author Corey Ford <fordco@sonoma.edu>
 * @brief The implementation of the Grapheme class.
 * @see Grapheme.h for the interface and documentation.
 * @version $Id$
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

#include "Grapheme.h"
namespace OCR
{

/**
 * Determines whether a pixel is a foreground pixel.
 * @param pixel a pixel
 * @return whether this is a foreground pixel
 */
bool isForeground(RGBApixel * pixel)
{
	return (pixel->Red < FG_THRESHOLD && pixel->Green < FG_THRESHOLD
			&& pixel->Blue < FG_THRESHOLD);
}

/**
 * Determines whether two pixels are similar in color
 * @param a first pixel
 * @param b second pixel
 * @return whether they should be considered similar
 */
bool isSimilar(RGBApixel * a, RGBApixel * b)
{
	int red = a->Red - b->Red;
	int green = a->Green - b->Green;
	int blue = a->Blue - b->Blue;
	if (red < 0)
		red = -red;
	if (green < 0)
		green = -green;
	if (blue < 0)
		blue = -blue;
	return (red < 10 && green < 10 && blue < 10);
}

/**
 * Initializes the object using an entire image
 * @param img reference to a BMP
 */
Grapheme::Grapheme(BMP & img) :
	image(img), left(0), top(0)
{
	right = image.TellWidth();
	bottom = image.TellHeight();
}

/**
 * Initializes the object using part of a BMP image
 * @param[in] img reference to BMP
 * @param[in] x1  left coordinate
 * @param[in] y1  top coordinate
 * @param[in] x2  right coordinate
 * @param[in] y2  bottom coordinate
 */
Grapheme::Grapheme(BMP & img, int x1, int y1, int x2, int y2) :
	image(img), left(x1), right(x2), bottom(y2), top(y1)
{
}

/**
 * Initializes the object as a copy of another
 * @param other a Grapheme object
 */
Grapheme::Grapheme(const Grapheme & other) :
	image(other.image), left(other.left), right(other.right), bottom(
			other.bottom), top(other.top)
{
}

/**
 * Copies the members of another object
 * @param other a Grapheme object
 */
Grapheme & Grapheme::operator =(const Grapheme & other)
{
	image = other.image;
	left = other.left;
	right = other.right;
	bottom = other.bottom;
	top = other.top;
	return *this;
}

/**
 * Recognizes the character.
 * @return the character
 * @todo Implement! This is the tricky part.
 */
char Grapheme::Read()
{
	pareDown();
	unsigned short holes = countHoles();
	std::vector<unsigned short> angles = findStraightLines();
	return 'a';
}

/**
 * Sets the left, right, bottom, and top coordinates
 * such that they surround the foreground pixels contained
 */
void Grapheme::pareDown()
{
	bool fgFound;
	// Pare top
	for (fgFound = false; top <= bottom && !fgFound; ++top)
		for (int col = left; col <= right; ++col)
			if (isForeground(image(col, top)))
				fgFound = true;
	// Pare bottom
	for (fgFound = false; bottom >= top && !fgFound; --bottom)
		for (int col = left; col <= right; ++col)
			if (isForeground(image(col, top)))
				fgFound = true;
	// Pare left
	for (fgFound = false; left <= right && !fgFound; ++left)
		for (int row = top; row <= bottom; ++row)
			if (isForeground(image(left, row)))
				fgFound = true;
	// Pare right
	for (fgFound = false; right >= left && !fgFound; --right)
		for (int row = top; row <= bottom; ++row)
			if (isForeground(image(right, row)))
				fgFound = true;
}
/**
 * Finds the extent of a contiguous shape
 * starting at a certain point
 * @param start starting Point
 * @return extent of shape
 */
Grapheme::Box Grapheme::findContiguousShape(const Point start)
{
	// Initialize queue and box
	std::queue<Point> Q;
	Box extent(start.x, start.y, start.x, start.y);
	// Keep track of which pixels have been visited
	bool ** visited;
	visited = new bool*[right - left];
	for (int i = 0; i < right - left; ++i)
		visited[i] = new bool[bottom - top];

	// Add the starting pixel to the queue
	Q.push(start);

	while (!Q.empty())
	{
		// Take a pixel
		Point p = Q.front();
		Q.pop();
		// Extend the current extent Box if necessary
		if (p.x < extent.low.x)
			extent.low.x = p.x;
		if (p.x > extent.high.x)
			extent.high.x = p.x;
		if (p.y < extent.low.y)
			extent.low.y = p.y;
		if (p.y > extent.high.y)
			extent.high.y = p.y;
		// Get all its neighbors
		int lowX = (p.x - 1 > left ? p.x - 1 : left);
		int highX = (p.x + 1 < right - 1 ? p.x + 1 : right - 1);
		int lowY = (p.y - 1 > top ? p.y - 1 : top);
		int highY = (p.y + 1 < bottom - 1 ? p.y + 1 : bottom);
		for (int x = lowX; x <= highX; ++x)
		{
			for (int y = lowY; y <= highY; ++y)
			{
				Point n(x, y);
				if (!visited[x - left][y - top] && isSimilar(image(start.x,
						start.y), image(n.x, n.y)))
				{
					Q.push(n);
					visited[x - left][y - top] = true;
				}
			}
		}
	}
	return extent;
}

/**
 * Determines whether one point is reachable
 * from another using pixels of similar color.
 * @param start starting point
 * @param end   starting point
 * @return
 */
bool Grapheme::isReachable(Point start, Point end)
{
	// Initialize queue and box
	std::queue<Point> Q;
	// Keep track of which pixels have been visited
	bool ** visited;
	visited = new bool*[right - left];
	for (int i = 0; i < right - left; ++i)
		visited[i] = new bool[bottom - top];

	// Add the starting pixel to the queue
	Q.push(start);

	while (!Q.empty())
	{
		// Take a pixel
		Point p = Q.front();
		Q.pop();
		// Is this the pixel we wanted?
		if (p == end)
			return true; // Yay
		// Get all its neighbors
		int lowX = (p.x - 1 > left ? p.x - 1 : left);
		int highX = (p.x + 1 < right - 1 ? p.x + 1 : right - 1);
		int lowY = (p.y - 1 > top ? p.y - 1 : top);
		int highY = (p.y + 1 < bottom - 1 ? p.y + 1 : bottom);
		for (int x = lowX; x <= highX; ++x)
		{
			for (int y = lowY; y <= highY; ++y)
			{
				Point n(x, y);
				if (!visited[x - left][y - top] && isSimilar(image(start.x,
						start.y), image(n.x, n.y)))
				{
					Q.push(n);
					visited[x - left][y - top] = true;
				}
			}
		}
	}
	// Now we have looked at all similar pixels reachable
	// from the starting point and have not found the end point
	return false;
}

/**
 * Counts the number of "holes" in the letter
 * @return number of holes
 */
unsigned short Grapheme::countHoles()
{
	// Try only some pixels
	int spacing = (right - left) / 10;
	// Determine which of these are unreachable
	std::set<Point> unreachables;
	Point corner(left, top);
	for (int x = left; x < right; x += spacing)
	{
		for (int y = top; y < bottom; y += spacing)
		{
			Point current(x, y);
			if (!isReachable(corner, current))
				unreachables.insert(current);
		}
	}
	// Check whether any of these pixels are reachable from each other (N! time)
	for (std::set<Point>::iterator i = unreachables.begin(); i != unreachables.end();)
	{
		// Make a copy of the iterator, then advance it
		std::set<Point>::iterator subject = i;
		++i;
		// Check all pixels from this point
		for (std::set<Point>::iterator j = i; j != unreachables.end(); ++j)
		{
			// and check whether they are reachable from the subject
			if (isReachable(*subject, *j))
			{
				unreachables.erase(*subject);
				break;
			}
		}
	}
	return unreachables.size();
}

/**
 * Finds the straight lines in the letter
 * @return angles of straight lines
 */
std::vector<unsigned short> Grapheme::findStraightLines()
{
}

} // namespace GraphemeResolver
