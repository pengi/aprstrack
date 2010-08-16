/*
    This file is part of APRStrack.

    Copyright (C) 2010 Max Sikström / SA6BBC

    APRStrack is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APRStrack is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APRStrack.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef TRIGGER_H
#define TRIGGER_H

void Trigger_Init( void );
void Trigger_Event( void ); // Define this outside timer lib

#endif
