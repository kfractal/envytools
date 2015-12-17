This tool reads rnndb's XML database beginning with the root file "root.xml".
The intent then is to merge the hwref information from NVIDIA into rnndb.

Qt >= 5.x should be installed in order to build and run (tested with 5.5).
With that in mind, to build:

    qmake normalize.pro
    make [-k, -j*, etc.]

Run from the rnndb dir (..) so that "root.xml" is in the current directory.

    ./normalize/normalize_rnndb

As of latest check-in it should return with code 0 and produce output similar
to:

    "reg32: NV_MMIO::PUNK022::PDAEMON_ENABLE 0x00022210"
    "reg8: NV_MMIO::PROM 0x00610000"
    [...]

If someone adds a reference to an element or attribute this tool isn't
expecting a warning will be issued.  Likewise any xml files found at the root
dir of rnndb which *aren't* used by the hierarchy at root.xml will also
be warned of.

The XML schema in rules-ng.xsd is used to validate the set of xml files
referenced by root.xml without error.  A few changes to the schema and
the xml files were needed to make that happen.  In some cases, taking care
of missing attributes or elements.  And in some, changes to the schema were
necessary.  But, nothing major.

The next large step here is to take this code base and bring it up at run-time
alongside the tool which I used to generate the nvgpu->nouveau headers.
I'm working on that now... [tbd: bug/rfe reference]

The name chosen here, 'normalize' is only for lack of a better phrase.  :/
But, it is suggestive in the sense that once the tool completes its discovery
phase it can also re-generate the xml database.  And, done once, it should
produce the same result the next time.  So it "normalizes" it it that sense.
Also, by interpreting the existing xml database and attempting to merge it with
the internal NVIDIA headers we'll be reconciling or normalizing the two schemes.
