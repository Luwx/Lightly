<p align="center">
  <img src="logo.png"/>
</p>

*Lightly* is a fork of breeze theme style that aims to be visually modern and minimalistic. 

## Development ⚠️

Lightly is a work in progress theme, there is still a lot to change, so expect bugs! Some applications may suddenly crash or flicker.

If you have any feedback or encounter some bugs, feel free to open an issue or a pr.


## Screenshots

![screenshot](https://github.com/Luwx/Lightly/blob/master/Screenshot.png)


## Transparency

The Lightly configuration file can be found at ~/.config/lightlyrc (if it does not exist, just create it or change a setting in the lightly config page in system settings). To configure the toolbar and menubar transparency, add the following line below the ```[Style]``` section: ```ToolBarOpacity=100```, where **100** is totally opaque (and no blur) and **0** is completely transparent. This setting is not exposed in the config page, yet.

The entire window can also be made transparent and blurred, for this you will have to change the color scheme file directly in ~/.local/share/color-schemes. Open your desired color scheme and, in the ```[Colors:Window]``` section, add a fourth value to ```BackgroundNormal```, like ```BackgroundNormal=0,0,0,127``` where the last value is the alpha.

Likewise, the decoration background alpha can be changed in the ```[WM]``` section. 


## TO DO

- [ ] Refactor and simplify the code 
- [ ] Add animations to buttons and lineedits
- [ ] Fix animations of checkboxes and radiobuttons
- [ ] Add ticks to checkboxes
- [ ] Work on tab style
- [ ] Make the changes compatible with QtQuick applications
- [ ] Work on the window decoration style
- [ ] Support transparency and blur ([goal](https://github.com/Luwx/Lightly/blob/master/goal-decoration_and_toolbar.png)) ?
- [ ] Add settings to configure or disable the button shadow and other tweaks.


## Manual installation
```
git clone https://github.com/Luwx/Lightly.git
cd Lightly && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_TESTING=OFF ..
make
sudo make install
```

## Acknowledgments

Breeze authors and Kvantum developer Pedram Pourang.





