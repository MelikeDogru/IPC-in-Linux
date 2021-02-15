TARGET_01        = server
SOURCE_01        = server.c

CC = gcc
CFLAGS = -Wall -g3 -gdwarf-2 -DDEBUG
LDLIBS  = -lpthread -lrt

.PHONY: all
all: $(TARGET_01) $(TARGET_02)

$(TARGET_01): $(SOURCE_01)
	$(CC) $(CFLAGS) $(SOURCE_01) $(LIBS) -o $(TARGET_01)


.PHONY: clean
clean:
	rm $(TARGET_01) $(OBJS)
