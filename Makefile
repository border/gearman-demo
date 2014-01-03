GEARMAN_PATH := /usr/local/opt/gearman

FLGAS := -g -std=gnu99

LDFLAGS := $(LDFLAGS) -I$(GEARMAN_PATH)/include
LIBS := $(LIBS) -L$(GEARMAN_PATH)/lib -lgearman

all:
	gcc -o cworker cworker.c $(FLGAS) $(LDFLAGS) $(LIBS)
	gcc -o client_task client_task.c $(FLGAS) $(LDFLAGS) $(LIBS)

clean:
	rm -rf *.o cworker client_task

