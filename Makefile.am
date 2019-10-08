include $(top_srcdir)/Makefile.tool.am

EXTRA_DIST = docs/igd-manual.xml

#----------------------------------------------------------------------------
# instrgrind-<platform>
#----------------------------------------------------------------------------

noinst_PROGRAMS  = instrgrind-@VGCONF_ARCH_PRI@-@VGCONF_OS@
if VGCONF_HAVE_PLATFORM_SEC
noinst_PROGRAMS += instrgrind-@VGCONF_ARCH_SEC@-@VGCONF_OS@
endif

INSTRGRIND_SOURCES_COMMON = \
	groups.c \
	instrs.c \
	main.c \
	smarthash.c \
	smartlist.c

instrgrind_@VGCONF_ARCH_PRI@_@VGCONF_OS@_SOURCES      = \
	$(INSTRGRIND_SOURCES_COMMON)
instrgrind_@VGCONF_ARCH_PRI@_@VGCONF_OS@_CPPFLAGS     = \
	$(AM_CPPFLAGS_@VGCONF_PLATFORM_PRI_CAPS@)
instrgrind_@VGCONF_ARCH_PRI@_@VGCONF_OS@_CFLAGS       = $(LTO_CFLAGS) \
	$(AM_CFLAGS_@VGCONF_PLATFORM_PRI_CAPS@)
instrgrind_@VGCONF_ARCH_PRI@_@VGCONF_OS@_DEPENDENCIES = \
	$(TOOL_DEPENDENCIES_@VGCONF_PLATFORM_PRI_CAPS@)
instrgrind_@VGCONF_ARCH_PRI@_@VGCONF_OS@_LDADD        = \
	$(TOOL_LDADD_@VGCONF_PLATFORM_PRI_CAPS@)
instrgrind_@VGCONF_ARCH_PRI@_@VGCONF_OS@_LDFLAGS      = \
	$(TOOL_LDFLAGS_@VGCONF_PLATFORM_PRI_CAPS@)
instrgrind_@VGCONF_ARCH_PRI@_@VGCONF_OS@_LINK = \
	$(top_builddir)/coregrind/link_tool_exe_@VGCONF_OS@ \
	@VALT_LOAD_ADDRESS_PRI@ \
	$(LINK) \
	$(instrgrind_@VGCONF_ARCH_PRI@_@VGCONF_OS@_CFLAGS) \
	$(instrgrind_@VGCONF_ARCH_PRI@_@VGCONF_OS@_LDFLAGS)

if VGCONF_HAVE_PLATFORM_SEC
instrgrind_@VGCONF_ARCH_SEC@_@VGCONF_OS@_SOURCES      = \
	$(INSTRGRIND_SOURCES_COMMON)
instrgrind_@VGCONF_ARCH_SEC@_@VGCONF_OS@_CPPFLAGS     = \
	$(AM_CPPFLAGS_@VGCONF_PLATFORM_SEC_CAPS@)
instrgrind_@VGCONF_ARCH_SEC@_@VGCONF_OS@_CFLAGS       = $(LTO_CFLAGS) \
	$(AM_CFLAGS_@VGCONF_PLATFORM_SEC_CAPS@)
instrgrind_@VGCONF_ARCH_SEC@_@VGCONF_OS@_DEPENDENCIES = \
	$(TOOL_DEPENDENCIES_@VGCONF_PLATFORM_SEC_CAPS@)
instrgrind_@VGCONF_ARCH_SEC@_@VGCONF_OS@_LDADD        = \
	$(TOOL_LDADD_@VGCONF_PLATFORM_SEC_CAPS@)
instrgrind_@VGCONF_ARCH_SEC@_@VGCONF_OS@_LDFLAGS      = \
	$(TOOL_LDFLAGS_@VGCONF_PLATFORM_SEC_CAPS@)
instrgrind_@VGCONF_ARCH_SEC@_@VGCONF_OS@_LINK = \
	$(top_builddir)/coregrind/link_tool_exe_@VGCONF_OS@ \
	@VALT_LOAD_ADDRESS_SEC@ \
	$(LINK) \
	$(instrgrind_@VGCONF_ARCH_SEC@_@VGCONF_OS@_CFLAGS) \
	$(instrgrind_@VGCONF_ARCH_SEC@_@VGCONF_OS@_LDFLAGS)
endif


