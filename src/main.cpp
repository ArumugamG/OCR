/**
 * @file main.cpp
 * @date Spring 2009
 * @author Corey Ford <fordco@sonoma.edu>
 * @brief An application program using the OCR library.
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

#include <iostream>
#include <string>
#include "EasyBMP/EasyBMP.h"
#include "Font.h"
#include "Page.h"
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using OCR::Page;
using OCR::Font;

int main(int argc, char * argv[])
{
	string result;
	if (argc != 3)
	{
		cerr << "Usage: " << argv[0] << " <font name> <bitmap name>\n";
		return 1;
	}

	// Load the font
	Font f(argv[1]);

	// Load the page from a bitmap file
	BMP img;
	img.ReadFromFile(argv[2]);

	// Read the page
	Page page(img, f);
	result = page.Read();
	cout << result << endl;

	return 0;
}
