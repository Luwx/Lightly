<p align="center">
  <img src="logo.png"/>
</p>

*Lightly* is a theme style that aims to be visually modern and minimalistic. 

## Screenshots

![screenshot](https://github.com/Luwx/test/blob/master/Screenshot.png)


## TO DO

- [ ] Refactor and simplify the code 
- [ ] Add animations to buttons and lineedits
- [ ] Fix animations of checkboxes and radiobuttons
- [ ] Add ticks to checkboxes
- [ ] Work on tab style
- [ ] Make the changes compatible with QtQuick applications
- [ ] Work on the window decoration style
- [ ] Support transparency and blur ([goal](https://github.com/Luwx/test/blob/master/goal-decoration_and_toolbar.png)) ?
- [ ] Add settings to configure or disable the button shadow and other tweaks.


## Manual installation
```
git clone https://github.com/Luwx/Lightly.git
cd Lightly && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_TESTING=OFF ..
make
sudo make install
```







