This tool reads rnndb's XML database beginning with the root file "root.xml".
The intent then is to merge the hwref information from NVIDIA into rnndb.

Qt >= 5.x should be installed in order to buid and run (tested with 5.5).
With that in mind, to build:

    qmake normalize.pro
    make <-j*>

Run from the rnndb dir (..) so that "root.xml" is in the current directory.
    normalize/normalize_rnndb

As of latest check-in it should produce output along the lines of

    warning: ignored attribute "enum.bare"
    warning: ignored attribute "domain.varset"
    [...]

This is an indication of what bits of the xml haven't been scraped yet.
The next step is to take this code base and bring it up at run time with
the tool which I used to generate the nvgpu->nouveau headers.
I'm working on that now...

The name chosen here, 'normalize' is only for lack of a better phrase.  :/
