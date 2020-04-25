# Latex Pictures
For documentational purpose, some pictures are generated using Latex. This folder holds the Latex source to generate these pictures and the pictures themselves (so they may be linked to inside the repository).

The shell-script [generatePng.sh](./generatePng.sh) performs for each *.tex in the folder

* generate *.pdf
* crop
* convert to *.png
* delete intermediate files
