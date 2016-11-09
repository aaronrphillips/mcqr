bin_PROGRAMS       = mcqr

mcqr_SOURCES  =                               \
                     mcqueuer.cpp             \
                     global.cpp               \
                     osUtil.cpp               \
                     Controller.cpp           \
                     Dealer.cpp               \
                     Exception.cpp            \
                     MessageWrapper.cpp       \
                     num2string.cpp           \
                     ProgramOptions.cpp       \
                     SysStats.cpp             \
                     Worker.cpp               


mcqr_LDADD    =                              \
                     $(SIGAR_LIB_NAME)       \
                     $(SLURM_LIB)            \
                     -lzmq -ldl -lstdc++ -lpthread

mcqr_LDFLAGS  = -gdwarf-2 -gstrict-dwarf $(MCQUEUER_CXX_STATIC) -Wl,-rpath=$(prefix)/lib -Wl,-rpath=$(ZEROMQ_LIB)/lib -Wl,-rpath=$(SIGAR_LIB)/sigar-bin/lib


AM_CXXFLAGS        = -gdwarf-2 -gstrict-dwarf                                        \
                     $(MCQUEUER_CXX_INCLUDE)  $(MCQUEUER_VERSION)                    \
                     -I$(SIGAR_LIB)/sigar-bin/include                                \
                     -I$(SLURM_LIB)/include                                          \
                     -Waddress                                                       \
                     -Warray-bounds                                                  \
                     -Wc++11-compat                                                  \
                     -Wchar-subscripts                                               \
                     -Wenum-compare                                                  \
                     -Wcomment                                                       \
                     -Wformat                                                        \
                     -Wmain                                                          \
                     -Wmaybe-uninitialized                                           \
                     -Wmissing-braces                                                \
		     -Wno-deprecated-declarations                                    \
                     -Wnonnull                                                       \
                     -Wparentheses                                                   \
                     -Wreorder                                                       \
                     -Wreturn-type                                                   \
                     -Wsequence-point                                                \
                     -Wstrict-aliasing                                               \
                     -Wstrict-overflow=1                                             \
                     -Wswitch                                                        \
                     -Wtrigraphs                                                     \
                     -Wuninitialized                                                 \
                     -Wunknown-pragmas                                               \
                     -Wunused-label                                                  \
                     -Wunused-value                                                  \
                     -Wunused-variable                                               \
                     -Wvolatile-register-var                                         \
                     -Wunused-but-set-variable                                       \
                     -Wclobbered                                                     \
                     -Wempty-body                                                    \
                     -Wmissing-field-initializers                                    \
                     -Wtype-limits                                                   \
                     -Wuninitialized                                                 \
                     -Wunused-but-set-parameter                                      \
                     -Winit-self                                                     \
                     -Wsystem-headers                                                \
                     -Wwrite-strings                                                 

#                     -Wunused-function                                               \
#                     -Wignored-qualifiers                                       
#                     -Wunused-parameter                                         
#                     -Wsign-compare                                             
#                     -Wsizeof-pointer-memaccess
#                     -Wimplicit-int                                             
#                     -Wimplicit-function-declaration                            
#                     -Wpointer-sign                                             
#                     -Wmissing-parameter-type                                   
#                     -Wold-style-declaration                                    
#                     -Woverride-init                                            

AM_LDFLAGS         = $(MCQUEUER_CXX_LIB)


EXTRA_DIST         =                              \
                     Controller.hpp               \
                     Dealer.hpp                   \
                     Exception.hpp                \
                     global.hpp                   \
                     MessageWrapper.hpp           \
                     num2string.hpp               \
                     osUtil.hpp                   \
                     ProgramOptions.hpp           \
                     SysStats.hpp                 \
                     Worker.hpp                   


dist-hook: clean
	-rm -rf `find $(distdir) -type d -name .svn`
