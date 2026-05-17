#
# ++ start-license-description ++
#
# Copyright (c) 2026 Stefan Sinnige.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# ++ end-license-description ++
#

if HTML
    MUDDOC_HTML = muddoc-html
else
    MUDDOC_HTML =
endif

# HTML directories
refdir = html/reference

# noinst_SCRIPTS = $(MUDDOC_HTML)

# Define the mud components that hold all the header files to generate the
# documentation for.
COMPONENTS = \
	core \
	crypto \
	html \
	io \
	protocols \
	protocols/http \
	protocols/soap \
	protocols/tls \
	test \
	xml

# As muddoc is llvm-based, it operates as a c++ compiler and would require the
# same C++ compiler options. All the source-folders would act as include paths
# and are derived from the component name.
muddoc_html_CPPFLAGS = $(patsubst %,-I$(top_srcdir)/%/include,$(COMPONENTS))

# Search for all the header files to generate HTML documentation for
muddoc_html_SOURCES = $(foreach dir,$(COMPONENTS),$(shell find $(top_srcdir)/$(dir)/include -type f -name "*h" | sed 's|$(top_srcdir)/$(dir)/||'))

# All static HTML files
static_html = \
    $(refdir)/highlight.js \
	$(refdir)/mudlib.css \
	$(refdir)/mud-small.png

# All generated HTML files
gen_html = $(patsubst include/%.h,$(refdir)/%.html,$(muddoc_html_SOURCES))

# All the HTML data (generated and static)
html_DATA = $(static_html) $(gen_html)

# Generate dynamic makefile-rules for each source-directory
define generate_html
$(refdir)/mud/$(1).html: $(top_srcdir)/$(1)/include/mud/$(1).h
	@echo "Generating $$@"
	@$(MKDIR_P) $$(@D)
	@$(srcdir)/scripts/html-index.sh $$? | \
	    $(XSLTPROC) \
	        --stringparam header-file $$(patsubst $(refdir)/%.html,%.h,$$@) \
	        $(srcdir)/html-index.xsl - > $$@

$(refdir)/mud/$(1)/%.html: $(top_srcdir)/$(1)/include/mud/$(1)/%.h
	@echo "Generating $$@"
	@$(MKDIR_P) $$(@D)
	@$(MUDDOC) $$? \
	    -- \
	    $(MUDDOC_CXX_FLAGS) \
	    $(CXXFLAGS) $(CPPFLAGS) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	    $(muddoc_html_CPPFLAGS) | \
	    $(XSLTPROC) \
	        --stringparam header-file $$(patsubst $(refdir)/%.html,%.h,$$@) \
	        $(srcdir)/html.xsl - > $$@
endef
$(foreach dir,$(COMPONENTS),$(eval $(call generate_html,$(dir))))

# Clean all generated HTML
clean-local:
	rm -f $(gen_html)

# vi: set ai ts=4 noexpandtab: 
