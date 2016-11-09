DIST_DIR         =   /srv/lcs/warehouse/mcqueuer
ACLOCAL_AMFLAGS  = -I config/m4
EXTRA_DIST       =    COPYING                          \
                      README                           \
                      conf                             \
                      config/m4                        \
                      utilities                        \
                      viz                              \
                      VERSION                          

SUBDIRS          =    src 
DIST_SUBDIRS     =    src 

DUCKBLAST_CXX=`which $(CXX)`
DUCKBLAST_CXX_DIR=`dirname $(DUCKBLAST_CXX)`
DUCKBLAST_CXX_ROOT=`dirname $(DUCKBLAST_CXX_DIR)`
DUCKBLAST_CXX_INCLUDE=`-I$(DUCKBLAST_CXX_ROOT)/include`
DUCKBLAST_CXX_LIB=`-L$(DUCKBLAST_CXX_ROOT)/lib -L$(top_srcdir)/lib`


MAJOR_VERSION=`$(AWK) -F'_' '{print $$1}' $(top_srcdir)/VERSION`
MINOR_VERSION=`$(AWK) -F'_' '{print $$2}' $(top_srcdir)/VERSION`
MICRO_VERSION=`$(AWK) -F'_' '{print $$3}' $(top_srcdir)/VERSION`
NEW_MAJOR_VERSION=`$(AWK) -F'_' '{print $$1+1}' $(top_srcdir)/VERSION | bc`
NEW_MINOR_VERSION=`$(AWK) -F'_' '{print $$2+1}' $(top_srcdir)/VERSION | bc`
NEW_MICRO_VERSION=`$(AWK) -F'_' '{print $$3+1}' $(top_srcdir)/VERSION | bc`

dist-hook: clean
	-rm -f  `find $(distdir) -type f -name \*.o`
	-rm -rf `find $(distdir) -type d -name .svn`

.PHONY: macro micro minor release
 

major: clean
	@echo "$(NEW_MAJOR_VERSION)_0_0" > $(top_srcdir)/VERSION
	cd $(top_srcdir) && autoreconf --install && ./configure `grep with-boost config.log | sed -e "s/.*configure //"`


minor: clean
	@echo "$(MAJOR_VERSION)_$(NEW_MINOR_VERSION)_0" > $(top_srcdir)/VERSION
	cd $(top_srcdir) && autoreconf --install && ./configure `grep with-boost config.log | sed -e "s/.*configure //"`


micro: clean
	@echo "$(MAJOR_VERSION)_$(MINOR_VERSION)_$(NEW_MICRO_VERSION)" > $(top_srcdir)/VERSION
	cd $(top_srcdir) && autoreconf --install && ./configure `grep with-boost config.log | sed -e "s/.*configure //"`


release: dist
	mv $(distdir).tar.gz $(DIST_DIR) && rm -f $(DIST_DIR)/mcqueuer-stable.tar.gz && ln -s $(DIST_DIR)/$(distdir).tar.gz $(DIST_DIR)/mcqueuer-stable.tar.gz

install-data-local:
	-cp utilities/* ${prefix}/bin
