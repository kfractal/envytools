This tool requires use of the Qt framework to build and run.
Qt >= 5.x should be installed (tested with 5.5).
With that in mind:
    qmake normalize_rnndb.pro
    make <-j*>

run from the .. dir so that "root.xml" is in the current directory.
    normalize/normalize_rnndb

as of latest check-in it should produce output along the lines of

warning: ignored attribute "enum.bare"
warning: ignored attribute "domain.varset"
[...]

this is an indication of what bits of the xml haven't been scraped yet.
next step is to take this code base and bring it up at run time with
the tool which i used to generate the nvgpu->nouveau headers.
working on that now...
