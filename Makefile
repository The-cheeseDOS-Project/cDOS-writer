# cheeseDOS Writer -  Write cheeseDOS to a CD, DVD, or USB without compiling cheeseDOS 
# Copyright (C) 2025  Connor Thomson
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

CC = gcc
SU = sudo
NAME = cdos-writer
IN = main.c
OUT = /usr/local/bin/$(NAME)

all:
	$(CC) $(IN) -o $(NAME)
	$(SU) cp $(NAME) $(OUT)

compile:
	$(CC) $(IN) -o $(NAME)

clean:
	rm -f $(NAME)

install: all

remove:
	$(SU) rm -f $(OUT)

.PHONY: all compile clean install remove