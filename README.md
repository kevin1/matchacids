# matchacids
matchacids is a short and simple command-line program that we wrote for comparing two lists of amino acids and saving the common acids. It's been tested on OS X and Linux. We used it in our science fair project, Identifying Structurally Important Regions in the HIV-1 Particle as Drug Targets.

We've open sourced it so that others can verify our results. Also, it's our way of giving back to the open-source community, as our project relied heavily on open-source bioinformatics tools.

# Compiling
On an OS X or Linux machine, simply switch to the directory this repository is located in and type `make`. You need to have GCC installed; if you are using a different compiler, change the `CC` variable in Makefile.

To clean the build products, use `make clean`.

If you are using Windows, you are on your own.

# Usage
The program takes two input filepaths as arguments:
- a VMD salt bridges plugin formatted file, and
- a metaPocket 2.0 formatted file or a Conserved Domain Database (CDD) Search formatted file (currently unimplemented).

The help message in this program has some more information. To view it, run the program with `-h` or `--help`.

# License
None! This program is public domain. Seriously.

But if you do something cool with it, we'd love to see! Send us a link or something.

# Developers
Kevin Chen (@[kevin1](http://github.com/kevin1))

Edward Wang (@[drcaramelsyrup](http://github.com/drcaramelsyrup))