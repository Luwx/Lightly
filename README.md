<p align="center">
  <img src="logo.png"/>
</p>

*Lightly* is a fork of breeze theme style that aims to be visually modern and minimalistic. 

## Development ⚠️

Lightly is a work in progress theme, there is still a lot to change, so expect bugs! Some applications may suddenly crash or flicker.

If you have any feedback or encounter some bugs, feel free to open an issue or a pr.


## Screenshots

![default](https://github.com/Luwx/Lightly/blob/master/Lightly-default.png)

With transparent dolphin view off and no sidebar transparency:
![custom](https://github.com/Luwx/Lightly/blob/master/Lightly-custom.png)

With a full glass color scheme (currently full glass color schemes are very buggy and not fully supported):
![fullglass](https://github.com/Luwx/Lightly/blob/master/Lightly-fullglass.png)

## Configuration

![config page](https://github.com/Luwx/Lightly/blob/master/config.png)

Lightly configuration page can be found in the KDE system settings under the Application style section. 


Most of these options are inherited from Breeze style, but Lightly has a few exclusive options that are enabled by default, including:

* Transparent Dolphin view (under the **frames** tab). This option disables the background and shadows of Dolphin view widget and draws top and bottom separators when the view has scrollable content.

*  Sidebar opacity (under the **transparency** tab). By default, it's 60. If it's bellow 100, shadows will be drawn automatically. 

The toolbar and menubar will follow the **titlebar** opacity. To configure the titlebar opacity, you will have to change the color scheme file directly in ~/.local/share/color-schemes. Open your desired color scheme and, in the ```[WM]``` section, add a fourth value to ```activeBackground``` and ```inactiveBackground```, like ```activeBackground=0,0,0,127``` where the last value is the alpha, that ranges from 0 (completely transparent) to 255 (totally opaque).

## Installation

### Installing Lightly from [chaotic-aur](https://aur.chaotic.cx/) on ArchLinux and its [derivatives](https://wiki.archlinux.org/title/Arch-based_distributions)
  1. Follow the instructions [here](https://aur.chaotic.cx/) on how to setup chaotic-aur 
  2. run: ```sudo pacman -Syyu lightly-qt```

### Installing Lightly from RPM repository on openSUSE Tumbleweed:

1. Add "sputnik-look-and-feel" repository:
```sudo zypper ar -ef https://download.opensuse.org/repositories/home:/sputnik:/look-and-feel/openSUSE_Tumbleweed/ sputnik-look-and-feel```
2. Refresh repository list:
```sudo zypper ref```
3. You will be notified about received new repository key. And you will be asked if you want to accept the key. This repository is created on OBS. All builds are reproducible.
Type "```a```" and press enter.
4. Install Lightly:
```sudo zypper in Lightly```

### Fedora
```
sudo dnf install Lightly
```

### Fedora 32 RPM repository

1. Create repo file:
```
echo  "[sputnik-look-and-feel]
name=Look And Feel (Fedora_32)
type=rpm-md
baseurl=https://download.opensuse.org/repositories/home:/sputnik:/look-and-feel/Fedora_32/
gpgcheck=1
gpgkey=https://download.opensuse.org/repositories/home:/sputnik:/look-and-feel/Fedora_32/repodata/repomd.xml.key
enabled=1" | sudo tee -a /etc/yum.repos.d/sputnik-look-and-feel.repo
```
2. Install Lightly:
```sudo dnf install Lightly```



## Manual installation

### Dependencies

Taken from https://github.com/n4n0GH/hello

#### Ubuntu
```
sudo apt install cmake build-essential libkf5config-dev libkdecorations2-dev libqt5x11extras5-dev qtdeclarative5-dev extra-cmake-modules libkf5guiaddons-dev libkf5configwidgets-dev libkf5windowsystem-dev libkf5coreaddons-dev libkf5iconthemes-dev gettext qt3d5-dev
```

#### Arch Linux
```
sudo pacman -S cmake extra-cmake-modules kdecoration qt5-declarative qt5-x11extras
```

#### Fedora
```
sudo dnf install cmake extra-cmake-modules "cmake(Qt5Core)" "cmake(Qt5Gui)" "cmake(Qt5DBus)" "cmake(Qt5X11Extras)" "cmake(KF5GuiAddons)" "cmake(KF5WindowSystem)" "cmake(KF5I18n)" "cmake(KDecoration2)" "cmake(KF5CoreAddons)" "cmake(KF5ConfigWidgets)" "cmake(Qt5UiTools)" "cmake(KF5GlobalAccel)" "cmake(KF5IconThemes)" kwin-devel libepoxy-devel "cmake(KF5Init)" "cmake(KF5Crash)" "cmake(KF5KIO)" "cmake(KF5Notifications)" kf5-kpackage-devel
```

#### openSUSE
```
sudo zypper install cmake gcc-c++ extra-cmake-modules libQt5Gui-devel libQt5DBus-devel libqt5-qttools-devel libqt5-qtx11extras-devel libQt5OpenGL-devel libQt5Network-devel libepoxy-devel kconfig-devel kconfigwidgets-devel kcrash-devel kglobalaccel-devel ki18n-devel kio-devel kservice-devel kinit-devel knotifications-devel kwindowsystem-devel kguiaddons-devel kiconthemes-devel kpackage-devel kwin5-devel xcb-util-devel xcb-util-cursor-devel xcb-util-wm-devel xcb-util-keysyms-devel
```

#### Solus
```
sudo eopkg install extra-cmake-modules kdecoration-devel qt5-declarative-devel qt5-x11extras-devel qt5-base-devel kcoreaddons-devel kguiaddons-devel kconfigwidgets-devel kwindowsystem-devel ki18n-devel kiconthemes-devel kcmutils-devel libxcb-devel xcb-util-devel qt5-wayland-devel kwayland-devel wayland-devel frameworkintegration-devel
```


### Build and install

```
git clone --single-branch --depth=1 https://github.com/Luwx/Lightly.git
cd Lightly && mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_INSTALL_LIBDIR=lib -DBUILD_TESTING=OFF ..
make
sudo make install
```

### Uninstall

In the build folder:
```
sudo make uninstall
```

## Acknowledgments

Breeze authors and Kvantum developer Pedram Pourang.





