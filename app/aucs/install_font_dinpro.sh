#!/bin/bash

cd /home/mint/devel/pin_machine/app/aucs

unzip DINPro.zip -d /tmp/din

cd /tmp/din

sudo mv *.ttf /usr/share/fonts/truetype/
sudo mv *.otf /usr/share/fonts/opentype/

sudo fc-cache -f -v
