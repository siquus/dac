#!/bin/bash

for file in *.tex; do
    name=$(echo "$file" | cut -f 1 -d '.')
    pdflatex $file;
    pdfcrop $name.pdf;
    pdftoppm $name-crop.pdf|pnmtopng > $name.png;
    rm $name.pdf;
    rm $name-crop.pdf;
done

latexmk -c;
