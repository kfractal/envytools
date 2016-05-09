This tool reads the rnndb XML database beginning with the root file "root.xml".
Then NVIDIA internal information is applied to the result and re-emitted in
the original XML form.

Qt >= 5.6 and clang++ >= 3.6 should be installed in order to build and run.  Note
however that only those with access to NVIDIA internal headers can run this tool
successfully.

The latest results of such a run have been checked-in with the tool, though.  See:
    'results/root.xml'

With that in mind, to build:

    qmake normalize.pro
    make [-k, -j*, etc.]

Run from the rnndb dir (..) so that "root.xml" is in the current directory.

    # put clang++ in PATH somehow
    ./normalize/normalize_rnndb

As of latest check-in it should return with code 0 and produce a directory
called 'results/' and emit something like this to stdout:

	[...]
	2587	tree def names
	609	tree reg names
	976	tree field names
	979	tree constant names
	[...]


The XML schema in rules-ng.xsd is used to validate the set of xml files
referenced by root.xml without error.  A few changes to the schema and
the xml files were needed to make that happen.  In some cases, taking care
of missing attributes or elements.  And in some, changes to the schema were
necessary.  But, nothing major.

The name chosen here, 'normalize' is only for lack of a better phrase.  :/
But, it is suggestive in the sense that once the tool completes its discovery
phase it can also re-generate the xml database.  And, done once, it should
produce the same result the next time.  So it "normalizes" it it that sense.
Also, by interpreting the existing xml database and attempting to merge it with
the internal NVIDIA headers we'll be reconciling or normalizing the two schemes.
