Simple plugin that loads an animated gif as your background.

Requirements:

ubuntu/debian:
sudo apt-get install kdebase-workspace-dev

opensuse:
sudo zypper install kdebase4-workspace-devel

To build:

mkdir Build
cd Build
cmake .. -DCMAKE_INSTALL_PREFIX=`kde4-config --localprefix`
make install

To reload plasma:
kquitapp plasma-desktop; sleep 1; plasma-desktop
