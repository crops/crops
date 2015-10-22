SUBDIRS = turff ceed codi

.PHONY: subdirs $(SUBDIRS)

all: TARGET = all

clean: TARGET = clean

debug: TARGET = debug

all clean debug: subdirs $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(TARGET)

