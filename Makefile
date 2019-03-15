# Copyright (C) 2019 Indeed Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
# in compliance with the License. You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software distributed under the
# License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
# express or implied. See the License for the specific language governing permissions and
# limitations under the License.
#
# Author: "Dave Chiluk"


# Declaration of variables
CC = gcc
CFLAGS ?= -Wall -Werror 
# CFLAGS += -std=gnu99
DEFINES = 

# File names
EXEC=fibtest
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
 
# Main target
$(EXEC): $(OBJECTS)
	$(CC) -pthread $(OBJECTS) -o $(EXEC)

%.o: %.c
	$(CC) -std=gnu99 -O3 $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXEC) $(OBJECTS)

