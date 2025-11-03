VERSION = 2.0

CPPFLAGS = -DVERSION=\"$(VERSION)\"
CFLAGS :=  -Wall -Wextra -O1
#CFLAGS :=  -Wall -Wextra -ggdb -fsanitize=address

CC ?= cc
